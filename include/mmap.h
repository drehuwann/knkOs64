#pragma once

#include "typedefs.h"
#include "textprint.h"

typedef struct mmapentry {
    u64 baseAddress;
    u64 regionLength;
    u32 regionType;
    u32 extendedAttributes;
} mmapentry;

extern u8 memRegionCnt;
extern u8 usableMemRegionsCnt;

void printmmap(mmapentry *mmap, u16 pos);
mmapentry **getUsableMemRegions();
mmapentry *getLargestUsableMemRegion();
