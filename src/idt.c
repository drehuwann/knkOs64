#include "idt.h"

__attribute__((aligned(0x10))) static idt64 _idt[IDT_MAX_DESCRIPTORS];

static idtr64 idtr;

extern isr IRQ_00h_x64, IRQ_01h_x64, IRQ_02h_x64, IRQ_03h_x64,
    IRQ_04h_x64, IRQ_05h_x64, IRQ_06h_x64, IRQ_07h_x64, IRQ_08h_x64,
    IRQ_0ah_x64, IRQ_0bh_x64, IRQ_0ch_x64, IRQ_0dh_x64, IRQ_0eh_x64,
    IRQ_10h_x64, IRQ_11h_x64, IRQ_12h_x64, IRQ_13h_x64, IRQ_14h_x64,
    isr0, isr1;

static bool vectors[IDT_MAX_DESCRIPTORS];
static isr isrs[IDT_MAX_DESCRIPTORS];

void idt_set_descriptor(u8 vector, void *isr, u8 flags) {
    idt64 *descriptor = &(_idt[vector]); 
    descriptor->offset_low = (u16)((u64)isr & 0xffff);
    descriptor->selector = 0x0008;  //codeSeg
    descriptor->ist = 0x00;
    descriptor->type_attr = flags;
    descriptor->offset_mid = (u16)(((u64)isr >> 16) & 0xffff);
    descriptor->offset_hi = (u32)(((u64)isr >> 32) & 0xffffffff);
    descriptor->zero = 0x00000000;
}

void initIdt() {
//    klog("in 'void initIdt()'", __FILE__, __LINE__, DEBUG);
    idtr.limit = (u16)(16 * 256 - 1);
    idtr.base = (u64)&_idt[0];

    // exceptions
    for (u8 vector = 0; vector < 32; vector ++) {
        if (vector == 0x08 || vector >= 0x0a && vector <= 0x0e ||
                vector == 0x11 || vector == 0x15) {
            // exception pushes errorcode
            idt_set_descriptor(vector, exc_dflt_handler_err, IDT_GATE_TRAP);
        } else {
            idt_set_descriptor(vector, exc_dflt_handler, IDT_GATE_TRAP);
        }
    }
    remapPic(); //init PIC1 @ 0x20 and PIC2 @0x28
    for (u16 vec = 32; vec < IDT_MAX_DESCRIPTORS; vec ++) {
        idt_set_descriptor((u8)(vec & 0xff), int_dflt_handler, IDT_GATE_INTR);        
    }
/*
    outb(PIC1_DAT, 0xfd);
    outb(PIC2_DAT, 0xff);
*/
    __asm__ volatile ("lidt %0"::"m"(idtr));
}
