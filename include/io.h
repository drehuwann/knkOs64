#pragma once

#include "typedefs.h"

#define PIC1_CMD    0x20
#define PIC1_DAT    0x21
#define PIC2_CMD    0xa0
#define PIC2_DAT    0xa1

#define ICW1_INIT   0x10
#define ICW1_ICW4   0x01
#define ICW4_8086   0x01

void outb(u16 port, u8 val);
u8 inb(u16 port);
void remapPic();
