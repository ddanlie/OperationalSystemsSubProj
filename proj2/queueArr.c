#include "queueArr.h"

struct queue
{
    unsigned first;
    unsigned last;
    unsigned size;
    unsigned capacity;
    pid_t *arr;
};

queue_t* qinit(unsigned capacity)
{
    queue_t *q = getSharedMem(sizeof(queue_t));
    q->arr = getSharedMem(sizeof(pid_t)*capacity);
    q->first = 0;
    q->last = 0;
    q->size = 0;
    q->capacity = capacity;
    return q;
}

void qadd(queue_t *q, pid_t data)
{
    if(q->last == q->capacity)
        q->last = 0;
    q->arr[q->last++] = data;
    q->size++;
}

pid_t qget(queue_t *q)
{
    if(q->first == q->capacity)
        q->first = 0;
    pid_t tmp = q->arr[q->first];
    q->arr[q->first] = -1;
    q->first++;
    q->size--;
    return tmp;
}

unsigned qsize(queue_t *q)
{
    return q->size;
}