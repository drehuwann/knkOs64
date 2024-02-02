#pragma once

#include "typedefs.h"
#include "io.h"
#include "idt.h"
#include "kbscancodeset1.h"
#include "debug_regs.h"
#include "pit.h"
#include "textprint.h"
#include "klog.h"

extern void (*mainkbhandler)(u8 scancode, u8 chr);

typedef struct int_frame {
    u64 rip;
    u64 cs;
    u64 rflags;
    u64 rsp;
    u64 ss;
} __attribute__((packed)) int_frame;

typedef struct int_frame_err {
    u64 errorcode;
    u64 rip;
    u64 cs;
    u64 rflags;
    u64 rsp;
    u64 ss;
} __attribute__((packed)) int_frame_err;

__attribute__((interrupt)) void exc_dflt_handler(int_frame *frame);
__attribute__((interrupt)) void exc_dflt_handler_err(int_frame_err *frame);
__attribute__((interrupt)) void int_dflt_handler(int_frame *frame);

__attribute__((no_caller_saved_registers)) void pic_sendEOI(u8 irq);

/** @brief #DE handler. resolve by incrementing RIP*/
__attribute__((interrupt)) void exc00_handler(int_frame *frame);

/** @brief #DB handler. inspects DR[0..7] registers*/
__attribute__((interrupt)) void exc01_handler(int_frame *frame);

/** @brief #NMI handler. save context at entry and restore at iret*/
__attribute__((interrupt)) void exc02_handler(int_frame *frame);

/** @brief #BP software breakpoint handler */
__attribute__((interrupt)) void exc03_handler(int_frame *frame);

/** @brief #OF handler. emit warning */
__attribute__((interrupt)) void exc04_handler(int_frame *frame);

/** @brief #BR handler. */
__attribute__((interrupt)) void exc05_handler(int_frame *frame);

/** @brief #UD handler. */
__attribute__((interrupt)) void exc06_handler(int_frame *frame);

/** @brief #NM handler. */
__attribute__((interrupt)) void exc07_handler(int_frame *frame);

/** @brief #DF handler
 * @param frame always pushes 0x0000000000000000 as frame->error
 * */
__attribute__((interrupt)) void exc08_handler(int_frame_err *frame);

/** @brief #TS handler. */
__attribute__((interrupt)) void exc0a_handler(int_frame_err *frame);

/** @brief #NP handler. */
__attribute__((interrupt)) void exc0b_handler(int_frame_err *frame);

/** @brief #SS handler. */
__attribute__((interrupt)) void exc0c_handler(int_frame_err *frame);

/** @brief #GP handler. */
__attribute__((interrupt)) void exc0d_handler(int_frame_err *frame);

/** @brief #PF handler
 * @warning loads cr2 register with linear address that generated the exception
 * */
__attribute__((interrupt)) void exc0e_handler(int_frame_err *frame);

/** @brief #MF handler. */
__attribute__((interrupt)) void exc10_handler(int_frame *frame);

/** @brief #AC handler. */
__attribute__((interrupt)) void exc11_handler(int_frame_err *frame);

/** @brief #MC handler. */
__attribute__((interrupt)) void exc12_handler(int_frame *frame);

/** @brief #XM handler. */
__attribute__((interrupt)) void exc13_handler(int_frame *frame);

/** @brief #VE handler. */
__attribute__((interrupt)) void exc14_handler(int_frame *frame);


//irqs
/** @brief PIT_ch0 interrupt handler. */
__attribute__((interrupt)) void irq00_handler(int_frame *frame);

/** @brief keyboard interrupt handler. */
__attribute__((interrupt)) void irq01_handler(int_frame *frame);

/** @brief COM1 interrupt handler. */
__attribute__((interrupt)) void irq04_handler(int_frame *frame);

/** @brief RTC interrupt handler. */
__attribute__((interrupt)) void irq08_handler(int_frame *frame);

/** @brief ps/2 mouse interrupt handler. */
__attribute__((interrupt)) void irq0c_handler(int_frame *frame);
//syscalls