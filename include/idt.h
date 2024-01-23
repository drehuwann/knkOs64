#pragma once

#include "typedefs.h"

#include "textprint.h"
#include "io.h"
#include "kbscancodeset1.h"
#include "klog.h"

typedef struct idt64 {
    u16 offset_low;
    u16 selector;
    u8 ist;
    u8 type_attr;
    u16 offset_mid;
    u32 offset_hi;
    u32 zero;
} idt64;

extern void (*mainkbhandler)(u8 scancode, u8 chr);

void initIdt();
