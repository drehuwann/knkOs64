#include "mmap.h"

mmapentry *usableMemRegions[MAXMEMREGIONS];

extern u8 _kernend;
u64 *pmBitmapAddr = (u64 *)(&_kernend); //put bitmap just above the kernel

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

static inline void pmBitmap_set(u64 bit) {
    pmBitmapAddr[bit / 64] |= (1 << (bit % 64));
}

static inline void pmBitmap_unset(u64 bit) {
    pmBitmapAddr[bit / 64] &= ~ (1 << (bit % 64));
}

static inline bool pmBitmap_test(u64 bit) {
    return pmBitmapAddr[bit / 64] & (1 << (bit % 64));
}

/// @brief keeps track of max memory size, in 0x1000 sized pages,
/// or in bits in bitmap
static u64 blk_count = 0;

void pmBitmapInit() {
    mmapentry *entry = (mmapentry*)0x5000;
    blk_count = 0;
    for (u8 i = 0; i < memRegionCnt; i++) {
        u64 base = entry->baseAddress;
        u64 size = entry->regionLength;
        u32 type = entry->regionType;
        u64 offset = 0;
        while (offset < size) {
            if (type == 1) {
                pmBitmap_unset((base + offset) / 0x1000);
            } else {
                pmBitmap_set((base + offset) / 0x1000);
            }
            offset += 0x1000;
            blk_count ++;
        }
        entry ++;
    }
    // finally, make the pages occupied by kernel and by bitmap busy
    for (u64 i = 0;
            i <= ((u64)pmBitmapAddr / 0x1000) + ((blk_count / 64) / 0x1000);
            i++) {
        pmBitmap_set(i);
    }
}

u64 pmBitmap_first_free() {
	//! find the first free bit
	for (u64 i=0; i < blk_count / 64; i++) {
		if (pmBitmapAddr[i] != 0xffffffffffffffff) {
			for (int j=0; j<64; j++) {		//! test each bit in the qword
				int bit = 1 << j;
				if (! (pmBitmapAddr[i] & bit) ) {
					return i * 64 + j;
                }
			}
        }
    }
    return 0; //out of memory
}

void *pm_alloc_block() {
	u64 frame = pmBitmap_first_free();
	if (frame == 0) return (void *)0;	//out of memory
    pmBitmap_set(frame);
    void *addr = (void *)(frame * 0x1000);
    return addr;
}

void pm_free_block (void* p) {
    if (p <= (void *)(pmBitmapAddr + (blk_count / 64)))
        return; //protected zone. do nothing
        //TODO protect hardware mapped adresses
    u64 frame = (u64)p / 0x1000;
    pmBitmap_unset(frame);
}
