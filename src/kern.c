#include "textprint.h"
#include "klog.h"
#include "pit.h"
#include "idt.h"
#include "interrupts.h"
#include "kbhandler.h"
#include "mmap.h"
#include "heap.h"

void _start() {
    logLvl llvl = DEBUG;
    clearScreen();
    setCursorPosition(posFromCoords(0, 0));
    printComPorts();
    if (initComPort(1)) {
        printStr("COM1 serial port succesfully initialized. ");
        setCurrentLogLevel(llvl);
        printStr("Log level set to ");
        printStr(lvlStr[llvl]);
    } else {
        printStrC("Couldn't initialize COM1 serial port",
            BACKGROUND_BLACK | FOREGROUND_RED);
    }
    printStr("\r\n");
    initIdt();
    idt_set_descriptor(0x00, exc00_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x01, exc01_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x02, exc02_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x03, exc03_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x04, exc04_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x05, exc05_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x06, exc06_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x07, exc07_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x08, exc08_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x0a, exc0a_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x0b, exc0b_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x0c, exc0c_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x0d, exc0d_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x0e, exc0e_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x10, exc10_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x11, exc11_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x12, exc12_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x13, exc13_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x14, exc14_handler, IDT_GATE_TRAP);

    //IRQs
    idt_set_descriptor(0x20, irq00_handler, IDT_GATE_INTR);
    idt_set_descriptor(0x21, irq01_handler, IDT_GATE_INTR);
//    idt_set_descriptor(0x22, irq02_handler, IDT_GATE_INTR);
//    idt_set_descriptor(0x23, irq03_handler, IDT_GATE_INTR);
    idt_set_descriptor(0x24, irq04_handler, IDT_GATE_INTR);
//    idt_set_descriptor(0x25, irq05_handler, IDT_GATE_INTR);
//    idt_set_descriptor(0x26, irq06_handler, IDT_GATE_INTR);
//    idt_set_descriptor(0x27, irq07_handler, IDT_GATE_INTR);
    idt_set_descriptor(0x28, irq08_handler, IDT_GATE_INTR);
//    idt_set_descriptor(0x29, irq09_handler, IDT_GATE_INTR);
//    idt_set_descriptor(0x2a, irq0a_handler, IDT_GATE_INTR);
//    idt_set_descriptor(0x2b, irq0b_handler, IDT_GATE_INTR);
    idt_set_descriptor(0x2c, irq0c_handler, IDT_GATE_INTR);
//    idt_set_descriptor(0x2d, irq0d_handler, IDT_GATE_INTR);
//    idt_set_descriptor(0x2e, irq0e_handler, IDT_GATE_INTR);
//    idt_set_descriptor(0x2f, irq0f_handler, IDT_GATE_INTR);
    
    // syscalls handlers come there
    
    __asm__ volatile("sti");

    SetFrequency(100); //init kernel timer to 100 Hz
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
//test #DE exception
    u8 ii = 1;
    while (ii) {
        ii--;
        printStr("10 / ");
        printStr(int2str((s64)ii));
        printStr(" = ");
        printStr(int2str(10 / ii));
        printStr("\n\r");
    }
// test #DB exception
    asm volatile("int1");
// test #NMI
    asm volatile("int $2");
// test #BP
    asm volatile("int3");
// test #OF
    u8 i = 220;
    while ((i+=20) && (i < 250)) {
        asm volatile("jno 1f;int $4;1:"); //'into' equiv. in 64 bits mode
    }
// test #BR
    asm volatile("int $5");
// test #DF
    asm volatile("int $8");
    return;
}
