#include "queue.h"

typedef struct qlist_st
{
    qtype_t data;
    struct qlist_st *next;
} qlist_t;

struct queue_st
{
    unsigned size;
    qlist_t *first;
    qlist_t *last;
};

queue_t* qinit()
{
    queue_t *newq = malloc(sizeof(queue_t));
    newq->size = 0;
    newq->first = NULL;
    newq->last = NULL;
    return newq;
}

void qfree(queue_t* queue)
{
    queue->first = NULL;
    queue->last = NULL;
    qlist_t *head = queue->first;
    qlist_t *next = queue->first->next;
    while(next != NULL)
    {
        free(head);
        head = next;
        next = head->next;
    }
    free(head);
    free(queue);
}

void qadd(queue_t *queue, qtype_t data)
{
    if(queue->size == 0)
    {
        queue->first = malloc(sizeof(qlist_t));
        queue->first->data = data;
        queue->last = queue->first;
        queue->last->next = NULL;
    }
    else
    {
        queue->last->next = malloc(sizeof(qlist_t));
        queue->last = queue->last->next;
        queue->last->data = data;
        queue->last->next = NULL;
    }
    queue->size += 1;
}

qtype_t qget(queue_t *queue)
{
    if(queue->first == NULL)
        return 0;

    qtype_t tmpData = queue->first->data;
    qlist_t *tmpElem = queue->first;
    queue->first =  queue->first->next;
    free(tmpElem);
    queue->size -= 1;
    return tmpData;
}

unsigned qsize(queue_t *queue)
{
    return queue->size;
}