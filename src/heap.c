#include "heap.h"

memseghdr *firstFreeMemSeg = (memseghdr *)0;

void initHeap(u64 heapAddr, u64 heapLength) {
    firstFreeMemSeg = (memseghdr *)heapAddr;
    firstFreeMemSeg->memLength = heapLength - sizeof(memseghdr);
    firstFreeMemSeg->nextSeg = (memseghdr *)0;
    firstFreeMemSeg->prevSeg = (memseghdr *)0;
    firstFreeMemSeg->nextFreeSeg = (memseghdr *)0;
    firstFreeMemSeg->prevFreeSeg = (memseghdr *)0;
    firstFreeMemSeg->isFree = 1;
}

void *malloc(u64 size) {
    u64 remain = size % 8;
    size -= remain;
    if (remain != 0) size += 8; //alignment

    memseghdr *currMemSeg = firstFreeMemSeg;
    if (currMemSeg == (memseghdr *)0) return (void *)0; //no free mem available
    while (1) {
        if (currMemSeg->memLength >= size) {
            if (currMemSeg->memLength > size + sizeof(memseghdr)) {
                memseghdr *newSegHdr = (memseghdr *)((u64)currMemSeg
                    + sizeof(memseghdr) + size);
                newSegHdr->isFree = 1;
                newSegHdr->memLength =((u64)currMemSeg->memLength
                    - (sizeof(memseghdr) + size));
                newSegHdr->nextFreeSeg = currMemSeg->nextFreeSeg;
                newSegHdr->nextSeg = currMemSeg->nextSeg;
                newSegHdr->prevSeg = currMemSeg;
                newSegHdr->prevFreeSeg = currMemSeg->prevFreeSeg;
                currMemSeg->nextFreeSeg = newSegHdr;
                currMemSeg->nextSeg = newSegHdr;
                currMemSeg->memLength = size;
            }
            if (currMemSeg == firstFreeMemSeg) {
                firstFreeMemSeg = currMemSeg->nextFreeSeg;
            }
            currMemSeg->isFree = 0;
            currMemSeg->memLength = size;
            if (currMemSeg->prevFreeSeg != (memseghdr *)0) {
                currMemSeg->prevFreeSeg->nextFreeSeg = currMemSeg->nextFreeSeg;
            }
            if (currMemSeg->nextFreeSeg != (memseghdr *)0) {
                currMemSeg->nextFreeSeg->prevFreeSeg = currMemSeg->prevFreeSeg;
            }
            if (currMemSeg->prevSeg != (memseghdr *)0) {
                currMemSeg->prevSeg->nextFreeSeg = currMemSeg->nextFreeSeg;
            }
            if (currMemSeg->nextSeg != (memseghdr *)0) {
                currMemSeg->nextSeg->prevFreeSeg = currMemSeg->prevFreeSeg;
            }

            return currMemSeg + 1;
        }
        if (currMemSeg->nextFreeSeg == (memseghdr *)0) {
            return (void *)0; // no more memory available.
        }
        currMemSeg = currMemSeg->nextFreeSeg;
    }
    return (void *)0; // we shouldn't ever get here !!
}

void *calloc(u64 size) {
    void *toRet = malloc(size);
    if (toRet != (void *)0) {
        memset(toRet, 0, size);
    }
    return toRet;
}

void *calloct(u64 num, u64 size) {
    return calloc(num * size);
}

void *realloc(void *addr, u64 newsize) {
    if (addr == (void *)0) return addr;
    memseghdr *oldseghdr;
    alignedmemseghdr *amsh = (alignedmemseghdr *)addr - 1;
    if (amsh->isAligned) {
        oldseghdr = (memseghdr *)(u64)amsh->memSegHdrAddr;
    } else {
        oldseghdr = ((memseghdr *)addr) - 1;
    }
    u64 smallerSize = newsize;
    if (oldseghdr->memLength < newsize) smallerSize = oldseghdr->memLength;
    void *newmem = malloc(newsize);
    memcpy(newmem, addr, smallerSize);
    free(addr);
    return(newmem);
}

void combinefreesegs(memseghdr *a, memseghdr *b) {
    if (a == b) return;
    if (a == (memseghdr *)0) return;
    if (b == (memseghdr *)0) return;
    if (a < b) {
        a->memLength += b->memLength + sizeof(memseghdr);
        a->nextSeg = b->nextSeg;
        a->nextFreeSeg = b->nextFreeSeg;
        b->nextSeg->prevSeg = a;
        b->nextSeg->prevFreeSeg = a;
        b->nextFreeSeg->prevFreeSeg = a;
    } else {
        b->memLength += a->memLength + sizeof(memseghdr);
        b->nextSeg = a->nextSeg;
        b->nextFreeSeg = a->nextFreeSeg;
        a->nextSeg->prevSeg = b;
        a->nextSeg->prevFreeSeg = b;
        a->nextFreeSeg->prevFreeSeg = b;
    }
}

void *alignedAlloc(u64 alignment, u64 size) {
    u64 alignmentRemain = alignment % 8;
    alignment -= alignmentRemain;
    if (alignmentRemain != 0) alignment += 8;
    u64 sizeRemain = size % 8;
    size -= sizeRemain;
    if (sizeRemain != 0) size += 8;
    u64 fullsize = size + alignment;
    void *mallocVal = malloc(fullsize);
    u64 addr = (u64)mallocVal;
    u64 remain = addr % alignment;
    addr -= remain;
    if (remain != 0) {
        addr += alignment;
        alignedmemseghdr *amsh = (alignedmemseghdr *)addr -1;
        amsh->isAligned = 1;
        amsh->memSegHdrAddr = (u64)mallocVal - sizeof(memseghdr);
    }
    return (void *)addr;
}

void free(void *addr) {
    memseghdr *currmemseg;
    alignedmemseghdr *amsh = (alignedmemseghdr *)addr - 1;
    if (amsh->isAligned) {
        currmemseg = (memseghdr *)(u64)amsh->memSegHdrAddr;
    } else {
        currmemseg = ((memseghdr *)addr) - 1;
    }
    currmemseg->isFree = 1;
    if (currmemseg < firstFreeMemSeg) firstFreeMemSeg = currmemseg;
    if (currmemseg->nextFreeSeg != (memseghdr*)0) {
        if (currmemseg->nextFreeSeg->prevFreeSeg < currmemseg)
            currmemseg->nextFreeSeg->prevFreeSeg = currmemseg;
    }
    if (currmemseg->prevFreeSeg != (memseghdr*)0) {
        if (currmemseg->prevFreeSeg->nextFreeSeg > currmemseg)
            currmemseg->prevFreeSeg->nextFreeSeg = currmemseg;
    }
    if (currmemseg->nextSeg != (memseghdr*)0) {
        currmemseg->nextSeg->prevSeg = currmemseg;
        if (currmemseg->nextSeg->isFree) {
            combinefreesegs(currmemseg, currmemseg->nextSeg);
        }
    }
    if (currmemseg->prevSeg != (memseghdr*)0) {
        currmemseg->prevSeg->nextSeg = currmemseg;
        if (currmemseg->prevSeg->isFree) {
            combinefreesegs(currmemseg, currmemseg->prevSeg);
        }
    }   
}
