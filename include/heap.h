#pragma once

#include "typedefs.h"
#include "memory.h"

typedef struct memseghdr {
    u64 memLength;
    struct memseghdr *nextSeg;
    struct memseghdr *prevSeg;
    struct memseghdr *nextFreeSeg;
    struct memseghdr *prevFreeSeg;
    int isFree;
} memseghdr;

typedef struct alignedmemseghdr {
    u64 memSegHdrAddr : 63;
    int isAligned : 1;
} alignedmemseghdr;

void initHeap(u64 heapAddr, u64 heapLength);
void *malloc(u64 size);
void *calloc(u64 size);
void *calloct(u64 num, u64 size);
void *realloc(void *addr, u64 newsize);
void *alignedAlloc(u64 alignment, u64 size);
void free(void *addr);
