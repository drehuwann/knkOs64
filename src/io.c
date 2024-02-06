#include "io.h"

void outb(u16 port, u8 val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

u8 inb(u16 port) {
    u8 retVal;
    asm volatile ("inb %1, %0" : "=a"(retVal) : "Nd"(port));
    return retVal;
}
