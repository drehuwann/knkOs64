#include "textprint.h"
#include "klog.h"
#include "pit.h"
#include "idt.h"
#include "interrupts.h"
#include "kbhandler.h"
#include "mmap.h"
#include "heap.h"
#include "ring3.h"

extern u8 _kernSize;

extern u8 _btext;
extern u8 _etext;
extern u8 _bidt;
extern u8 _eidt;
extern u8 _brodata;
extern u8 _erodata;
extern u8 _bbss;
extern u8 _ebss;
extern u8 _bdata;
extern u8 _edata;

void _start() {
    logLvl llvl = DEBUG;
    clearScreen();
    setCursorPosition(posFromCoords(0, 0));
    printComPorts();
    if (initComPort(1)) {
        printStr((const u8 *)"COM1 serial port succesfully initialized. ");
        setCurrentLogLevel(llvl);
        printStr((const u8 *)"Log level set to ");
        printStr(logLvlStr(llvl));
    } else {
        printStrC((const u8 *)"Couldn't initialize COM1 serial port",
            BACKGROUND_BLACK | FOREGROUND_RED);
    }
    printStr((const u8 *)"\r\n");
    SetFrequency(100); //init kernel timer to 100 Hz
    initIdt();
    idt_set_descriptor(0x00, &exc00_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x01, &exc01_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x02, &exc02_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x03, &exc03_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x04, &exc04_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x05, &exc05_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x06, &exc06_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x07, &exc07_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x08, &exc08_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x0a, &exc0a_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x0b, &exc0b_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x0c, &exc0c_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x0d, &exc0d_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x0e, &exc0e_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x10, &exc10_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x11, &exc11_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x12, &exc12_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x13, &exc13_handler, IDT_GATE_TRAP);
    idt_set_descriptor(0x14, &exc14_handler, IDT_GATE_TRAP);

    //IRQs
    idt_set_descriptor(0x20, &irq00_handler, IDT_GATE_INTR);
    idt_set_descriptor(0x21, &irq01_handler, IDT_GATE_INTR);

    idt_set_descriptor(0x24, &irq04_handler, IDT_GATE_INTR);

    idt_set_descriptor(0x28, &irq08_handler, IDT_GATE_INTR);

    idt_set_descriptor(0x2c, &irq0c_handler, IDT_GATE_INTR);
    
    // syscalls handlers come there
    
    __asm__ volatile("sti");

    mainkbhandler = kbHandler;

    mmapentry **usablemmap = getUsableMemRegions();

    for (u8 i = 0; i < usableMemRegionsCnt; i++) {
        mmapentry *mmap = usablemmap[i];
        printmmap(mmap, cursorPosition);
    }

    mmapentry const *heapzone = getLargestUsableMemRegion();

    printStr((const u8 *)"Selected region @0x");
    printStr(hex2str((void *)(heapzone->baseAddress)));
    printStr((const u8 *)" for heap.\n\r");

    initHeap(heapzone->baseAddress, heapzone->regionLength);
    printStr((const u8 *)"Succesfully assigned ");
    printStr(int2str(heapzone->regionLength));
    printStr((const u8 *)" bytes at 0x");
    printStr(hex2str((void *)(heapzone->baseAddress)));
    printStr((const u8 *)" as heap.\n\rFeel free to malloc() !!!\n\r");
    printStr((const u8 *)"Warning : realloc() doesn't preserve alignment.\n\r");
    printStr(double2str(GetTimeSinceBoot(), 2));
    printStr((const u8 *)" seconds elapsed since boot\n\r");

//test varargs and kprintf
    s16 a = -32;
    const u8 st[] = "negative percentage";
    u8 yes = 'Y';
    u8 no = 'N';
    arg_list *p_arglist =
        arg_push((arg_list *)0, make_arg_vp((void **const)(&st)));
    if (!(arg_push(p_arglist, make_arg_sw(&a)))) asm volatile("int $0x0d");
    if (!(arg_push(p_arglist, make_arg_ub(&yes)))) asm volatile("int $0x0d");
    if (!(arg_push(p_arglist, make_arg_ub(&no)))) asm volatile("int $0x0d");
    kprintf((const u8 *)"Did you see this %s : %d%% (%c/%c) ?", p_arglist);
    printStr((const u8 *)"\n\r");
    if(p_arglist) free(p_arglist);
//TODO valgrind this !

    printStr(int2str((s64)(&_kernSize)));
    printStr((const u8 *)" : This is kernel size (in 512byte sectors)\n\r");
    printStr((const u8 *)" .text is from 0x");
    printStr(hex2str(&_btext));
    printStr((const u8 *)" to 0x");
    printStr(hex2str(&_etext));
    printStr((const u8 *)"\n\r");
    printStr((const u8 *)" .idt is from 0x");
    printStr(hex2str(&_bidt));
    printStr((const u8 *)" to 0x");
    printStr(hex2str(&_eidt));
    printStr((const u8 *)"\n\r");
    printStr((const u8 *)" .rodata is from 0x");
    printStr(hex2str(&_brodata));
    printStr((const u8 *)" to 0x");
    printStr(hex2str(&_erodata));
    printStr((const u8 *)"\n\r");
    printStr((const u8 *)" .bss is from 0x");
    printStr(hex2str(&_bbss));
    printStr((const u8 *)" to 0x");
    printStr(hex2str(&_ebss));
    printStr((const u8 *)"\n\r");
    printStr((const u8 *)" .data is from 0x");
    printStr(hex2str(&_bdata));
    printStr((const u8 *)" to 0x");
    printStr(hex2str(&_edata));
    printStr((const u8 *)"\n\r");

    pmBitmapInit();
    printStr((const u8 *)"the first free mem page is ");
    printStr(int2str((s64)pmBitmap_first_free()));
    printStr((const u8 *)"\n\r");

    void *p1 = pm_alloc_block();
    printStr((const u8 *)"allocated page @0x");
    printStr(hex2str(p1));
    printStr((const u8 *)"\n\r");
    void *p2 = pm_alloc_block();
    printStr((const u8 *)"allocated page @0x");
    printStr(hex2str(p2));
    printStr((const u8 *)"\n\r");
    pm_free_block(p1);
    printStr((const u8 *)"freed page @0x");
    printStr(hex2str(p1));
    printStr((const u8 *)"\n\r");
    p1 = pm_alloc_block();
    printStr((const u8 *)"allocated page @0x");
    printStr(hex2str(p1));
    printStr((const u8 *)"\n\r");
    void *p3 = (void *)0x2000;
    pm_free_block(p3);
    printStr((const u8 *)"tried to free page @0x");
    printStr(hex2str(p3));
    printStr((const u8 *)"\n\r");
    void *p4 = pm_alloc_block();
    printStr((const u8 *)"allocated page @0x");
    printStr(hex2str(p4));
    printStr((const u8 *)"\n\r");
    pm_free_block(p1); //freeing all
    pm_free_block(p2);
    pm_free_block(p4);

    printStr(double2str(GetTimeSinceBoot(), 2));
    printStr((const u8 *)" seconds elapsed since boot\n\r");

    jump_usermode();
    return;
}
