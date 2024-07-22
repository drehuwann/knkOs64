#pragma once

#include "typedefs.h"
#include "textprint.h"

typedef struct mmapentry {
    u64 baseAddress;
    u64 regionLength;
    u32 regionType;
    u32 extendedAttributes;
} __attribute__((packed)) mmapentry;

extern u8 memRegionCnt;
extern u8 usableMemRegionsCnt;

void printmmap(mmapentry *mmap, u16 pos);
mmapentry **getUsableMemRegions();
mmapentry *getLargestUsableMemRegion();

/// @brief sets physical memory bitmap at first aligned address above kernel
/// where each 0x1000-sized page is represented by a bit
/// (if this bit is equal to 1, it means the pege is busy)
void pmBitmapInit();

/// @brief bitmap should have been initialized prior to call this function
/// @return the first free page number. Returning 0 (ptotected page) means :
/// out of memory
u64 pmBitmap_first_free();

/// @brief allocates first free 0x1000-sized physical memory block
/// @return absolute address or nullptr if out of memory
void *pm_alloc_block();

/// @brief frees 0x1000-sized physical memory block containing p. Does nothing
/// if we try to free a protected zone (including kernel and bitmap)
/// @param p raw pointer
void pm_free_block(void *p);
