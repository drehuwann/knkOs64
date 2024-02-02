#pragma once

#include "typedefs.h"

#include "textprint.h"
#include "memory.h"
#include "interrupts.h"
#include "klog.h"

#define IDT_MAX_DESCRIPTORS 256
#define IDT_GATE_INTR 0b10001110
#define IDT_GATE_CALL 0b10001100
#define IDT_GATE_TRAP 0b10001111

#define IDT_GATE_TRAP_USR 0b11101110

typedef struct idt64 {
    u16 offset_low;
    u16 selector;
    u8 ist;
    u8 type_attr;
    u16 offset_mid;
    u32 offset_hi;
    u32 zero;
} __attribute__((packed)) idt64;

typedef struct {
	u16	limit;
	u64	base;
} __attribute__((packed)) idtr64;

typedef void (*isr)(void *frame);

void idt_set_descriptor(u8 vector, void* isr, u8 flags);

void initIdt();
