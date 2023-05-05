#include "mmap.h"

mmapentry *usableMemRegions[MAXMEMREGIONS];

void printmmap(mmapentry *mmap, u16 pos) {
    setCursorPosition(pos);
    printStr("Memory base : 0x");
    printStr(hex2strq(mmap->baseAddress));
    setCursorPosition(pos + VGAWIDTH);
    printStr("Region length : ");
    printStr(int2str(mmap->regionLength));
    setCursorPosition(pos + 2 * VGAWIDTH);
    printStr("Region type : ");
    printStr(int2str(mmap->regionType));
    setCursorPosition(pos + 3 * VGAWIDTH);
    printStr("Extended attributes : 0x");
    printStr(hex2strd(mmap->extendedAttributes));
    setCursorPosition(pos + 5 * VGAWIDTH);
}

int memRegionsGot = 0;
u8 usableMemRegionsCnt = 0;

mmapentry **getUsableMemRegions() {
    if (memRegionsGot) return usableMemRegions;
    u8 usableRegionIndex = 0;
    for(u8 i = 0; i < memRegionCnt; i++) {
        mmapentry *mmap = (mmapentry *)0x5000;
        mmap += i;
        if (mmap->regionType == 1) {
            usableMemRegions[usableRegionIndex ++] = mmap;
        }
    }
    usableMemRegionsCnt = usableRegionIndex;
    memRegionsGot = 1;
    return usableMemRegions;
}

mmapentry *getLargestUsableMemRegion() {
    mmapentry **usables = getUsableMemRegions();
    u64 sizemax = 0;
    mmapentry *toRet = (mmapentry *)0;
    for (u8 i = 0; i < usableMemRegionsCnt; i++) {
        if (usables[i]->regionLength > sizemax) {
            sizemax = usables[i]->regionLength;
            toRet = usables[i];
        }
    }
    return toRet;
}
