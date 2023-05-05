#include "textprint.h"
#include "idt.h"
#include "kbhandler.h"
#include "mmap.h"
#include "heap.h"

void _start() {
    clearScreen();
    setCursorPosition(posFromCoords(0, 0));
    initIdt();
    mainkbhandler = kbHandler;

    mmapentry **usablemmap = getUsableMemRegions();

    for (u8 i = 0; i < usableMemRegionsCnt; i++) {
        mmapentry *mmap = usablemmap[i];
        printmmap(mmap, cursorPosition);
    }

    mmapentry *heapzone = getLargestUsableMemRegion();

    printStr("Selected region @0x");
    printStr(hex2str((void *)(heapzone->baseAddress)));
    printStr(" for heap.\n\r");

    initHeap(heapzone->baseAddress, heapzone->regionLength);
    printStr("Succesfully assigned ");
    printStr(int2str(heapzone->regionLength));
    printStr(" bytes at 0x");
    printStr(hex2str((void *)(heapzone->baseAddress)));
    printStr(" as heap.\n\rFeel free to malloc() !!!\n\r");
    printStr("Warning : realloc() doesn't preserve alignment.\n\r");
    return;
}
