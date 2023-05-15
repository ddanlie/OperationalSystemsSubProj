#ifndef QARR_H
#define QARR_H

#include <unistd.h>
#include "shared.h"
#include <stdio.h>

typedef struct queue queue_t;

queue_t* qinit(unsigned capacity);
void qadd(queue_t *q, pid_t data);
pid_t qget(queue_t *q);
unsigned qsize(queue_t *q);

#endif