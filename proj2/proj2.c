#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include "shared.h"

char *outputFileName = "proj2.out";
FILE **outputFile;
unsigned long *counterA; //string counter
unsigned *counterNOM;    //created molecules counter
unsigned *molCount;      //calculated molecules amount
short *hcount;           //H count ready to create molecule
sem_t *dataMutex;        //synchronize to write and read data
sem_t *molMutexO;        //synchronize O order in queue
sem_t *molMutexH;        //synchronize H order in queue
sem_t *mutex1o;          //H(x2) waiting for signal from O
sem_t *mutex2h;          //O waiting for H(x2) from queue
unsigned *no;
unsigned *nh;
unsigned *ti;
unsigned *tb;

void oxygen(unsigned id);
void hydrogen(unsigned id);
unsigned calcMol();      //calculate molecules amount


int main(int argc, char *argv[])
{
    //allocate shared memory and assign variables
    argc = argc; //for compiler

    //assign variables uing shared memory
    counterA = getSharedMem(sizeof(unsigned long));
    *counterA = 1;
    counterNOM = getSharedMem(sizeof(unsigned));
    *counterNOM = 1;
    hcount = getSharedMem(sizeof(short));
    *hcount = 0;//current H in stack
    no = getSharedMem(sizeof(unsigned));
    *no = atoi(argv[1]);
    nh = getSharedMem(sizeof(unsigned));
    *nh = atoi(argv[2]);
    ti = getSharedMem(sizeof(unsigned));
    *ti = atoi(argv[3]);
    tb = getSharedMem(sizeof(unsigned));
    *tb = atoi(argv[4]);
    molCount = getSharedMem(sizeof(unsigned));
    *molCount = calcMol(*no, *nh);
    outputFile = getSharedMem(sizeof(FILE*));
    *outputFile = fopen(outputFileName, "w");
    dataMutex = getSharedMem(sizeof(sem_t));
    molMutexO = getSharedMem(sizeof(sem_t));
    molMutexH = getSharedMem(sizeof(sem_t));
    mutex1o = getSharedMem(sizeof(sem_t));
    mutex2h = getSharedMem(sizeof(sem_t));
    sem_init(dataMutex, 1, 1);
    sem_init(molMutexO, 1, 1);
    sem_init(molMutexH, 1, 2);
    sem_init(mutex1o, 1, 1);
    sem_init(mutex2h, 1, 1);
    sem_wait(mutex2h); //for first oxygen process
    sem_wait(mutex1o); //for first hydrogen process
    //launch O processes
    for(unsigned i = 0; i < *no; i++)
    {
        pid_t pid = fork();
        if(pid == 0)
            oxygen(i+1);
    }
    //launch H processes
    for(unsigned i = 0; i < *nh; i++)
    {
        pid_t pid = fork();
        if(pid == 0)
            hydrogen(i+1);
    }

    wait(NULL);
    fclose(*outputFile);
    return 0;
}

void oxygen(unsigned id)
{
    sem_wait(dataMutex);
    fprintf(*outputFile, "%lu: O %u: started\n", *counterA, id);
    fflush(*outputFile);
    (*counterA)++;
    sem_post(dataMutex);

    srand(time(NULL));
    usleep(rand()%((*ti)+1));

    sem_wait(dataMutex);
    fprintf(*outputFile, "%lu: O %u: going to queue\n", *counterA, id);
    fflush(*outputFile);
    (*counterA)++;
    sem_post(dataMutex);

    sem_wait(molMutexO); //creating O stack

    sem_wait(dataMutex);
    unsigned currCounterNOM = *counterNOM;
    if(currCounterNOM == (*molCount)+1)
    {
        fprintf(*outputFile, "%lu: O %u: not enough H\n", *counterA, id);
        fflush(*outputFile);
        (*counterA)++;
        sem_post(dataMutex);
        sem_post(molMutexO);
        exit(0);
    }
    fprintf(*outputFile, "%lu: O %u: creating molecule %u\n", *counterA, id, currCounterNOM);
    fflush(*outputFile);
    (*counterA)++;
    sem_post(dataMutex);

    sem_wait(mutex2h); // wait H x2 are ready to create molecule

    usleep(rand()%(*tb+1));

    sem_wait(dataMutex);
    fprintf(*outputFile, "%lu: O %u: molecule %u created\n", *counterA, id, currCounterNOM);
    fflush(*outputFile);
    (*counterA)++;
    (*counterNOM)++;
    sem_post(dataMutex);

    sem_post(mutex1o);
    sem_post(mutex1o);

    sem_post(molMutexO);
    exit(0);
}

void hydrogen(unsigned id)
{
    sem_wait(dataMutex);
    fprintf(*outputFile, "%lu: H %u: started\n", *counterA, id);
    fflush(*outputFile);
    (*counterA)++;
    sem_post(dataMutex);

    srand(time(NULL));
    usleep(rand()%((*ti)+1));

    sem_wait(dataMutex);
    fprintf(*outputFile, "%lu: H %u: going to queue\n", *counterA, id);
    fflush(*outputFile);
    (*counterA)++;
    sem_post(dataMutex);

    sem_wait(molMutexH);

    sem_wait(dataMutex);
    unsigned currCounterNOM = *counterNOM;
    if(currCounterNOM == (*molCount)+1)
    {
        fprintf(*outputFile, "%lu: H %u: not enough O or H\n", *counterA, id);
        fflush(*outputFile);
        (*counterA)++;
        sem_post(dataMutex);
        sem_post(molMutexH);
        exit(0);
    }
    fprintf(*outputFile, "%lu: H %u: creating molecule %u\n", *counterA, id, currCounterNOM);
    fflush(*outputFile);
    (*counterA)++;

    (*hcount)++;
    if(*hcount >= 2)
    {
        *hcount = 0;
        sem_post(mutex2h);
    }
    sem_post(dataMutex);

    sem_wait(mutex1o);

    sem_wait(dataMutex);
    fprintf(*outputFile, "%lu: H %u: molecule %u created\n", *counterA, id, currCounterNOM);
    fflush(*outputFile);
    (*counterA)++;
    sem_post(dataMutex);

    sem_post(molMutexH);

    exit(0);
}

unsigned calcMol()
{
    unsigned result;
    if(*no < *nh && *no <= *nh/2)
        result = *no;
    else //no >= nh || (no < nh && o > h/2)
        result = *nh/2;
    return result;
}