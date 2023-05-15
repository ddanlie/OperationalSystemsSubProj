#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

struct queue_st;
typedef struct queue_st queue_t;
typedef pid_t qtype_t;

queue_t* qinit();

void qfree(queue_t* queue);

void qadd(queue_t *queue, qtype_t data);

qtype_t qget(queue_t *queue);

unsigned qsize(queue_t *queue);

#endif