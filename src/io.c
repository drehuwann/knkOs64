#include "io.h"

void outb(u16 port, u8 val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

u8 inb(u16 port) {
    u8 retVal;
    asm volatile ("inb %1, %0" : "=a"(retVal) : "Nd"(port));
    return retVal;
}

void remapPic() {
    u8 a1, a2;

    a1 = inb(PIC1_DAT);
    a2 = inb(PIC2_DAT);

    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DAT, 0x00);
    outb(PIC2_DAT, 0x08);
    outb(PIC1_DAT, 0x04);
    outb(PIC2_DAT, 0x02);
    outb(PIC1_DAT, ICW4_8086);
    outb(PIC2_DAT, ICW4_8086);

    outb(PIC1_DAT, a1);
    outb(PIC2_DAT, a2);
}
