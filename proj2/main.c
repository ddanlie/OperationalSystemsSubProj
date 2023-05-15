#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "shared.h"
#include <semaphore.h>
void wait_input();

sem_t *mutex;

void foo()
{
    printf("foo ready\n");
    sem_wait(mutex);
    printf("foo end\n");
    exit(0);
}

void bar()
{
    printf("bar ready\n");
    sem_wait(mutex);
    printf("bar end\n");
    exit(0);
}

int main()
{
    mutex = getSharedMem(sizeof(sem_t));
    sem_init(mutex, 1, 1);
    pid_t pid = fork();
    if(pid == 0)
        foo();
    pid = fork();
    if(pid == 0)
        bar();
    wait(NULL);
    return 0;
}

void wait_input()
{
    char c;
    scanf("%c", &c);
}