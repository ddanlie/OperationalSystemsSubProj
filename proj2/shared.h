#ifndef SHARED_H
#define SHARED_H

#include <sys/mman.h>
#include <stdlib.h>

void* getSharedMem(unsigned size);

#endif