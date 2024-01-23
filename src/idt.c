#include "idt.h"

extern idt64 _idt[256];
extern u64 isr1;
extern void load_idt();

void (*mainkbhandler)(u8 scancode, u8 chr);

void initIdt() {
    klog("in 'void initIdt()'", __FILE__, __LINE__, DEBUG);

    _idt[1].zero = 0;
    _idt[1].offset_low = (u16)(((u64)&isr1 & 0x000000000000ffff));
    _idt[1].offset_mid = (u16)(((u64)&isr1 & 0x00000000ffff0000) >> 16);
    _idt[1].offset_hi = (u32)(((u64)&isr1 & 0xffffffff00000000) >> 32);
    _idt[1].ist = 0x00;
    _idt[1].selector = 0x08;
    _idt[1].type_attr = 0x8e;
    
    remapPic();

    outb(PIC1_DAT, 0xfd);
    outb(PIC2_DAT, 0xff);
    load_idt();
}

void isr1_handler() {
    u8 scancode = inb(0x60);
    u8 chr = 0x00;

    if (scancode < 0x50) chr = scanCodeLookupTable[scancode];
    if (mainkbhandler != 0) mainkbhandler(scancode, chr);

    outb(PIC1_CMD, 0x20);
    outb(PIC2_CMD, 0x20);
}
