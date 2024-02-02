#pragma once

#include "typedefs.h"

#include "klog.h"

#define PIC1_CMD        0x20
#define PIC1_DAT        0x21
#define PIC2_CMD        0xa0
#define PIC2_DAT        0xa1

#define PIC1_OFFSET     0x20    /* used by remapPic() */

#define PIC_EOI		    0x20	/* End-of-interrupt command code */
#define PIC_READ_IRR    0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR    0x0b    /* OCW3 irq service next CMD read */
 
#define ICW1_ICW4	    0x01	/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	    0x02	/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04	/* Call address interval 4 (8) */
#define ICW1_LEVEL	    0x08	/* Level triggered (edge) mode */
#define ICW1_INIT	    0x10	/* Initialization - required! */ 
#define ICW4_8086	    0x01	/* 8086/88 (MCS-80/85) mode */ 
#define ICW4_AUTO	    0x02	/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08	/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C	/* Buffered mode/master */
#define ICW4_SFNM	    0x10	/* Special fully nested (not) */

#define PIT_CH0         0x40    /* PIT channel0 port (r/w) routed to irq 0*/
#define PIT_CH1         0x41    /* PIT channel1 port (r/w), obsolete, unused */
#define PIT_CH2         0x42    /* PIT channel2 port (r/w) routed to speaker */
#define PIT_CMD         0x43    /* PIT command port (wo) */

void outb(u16 port, u8 val);
u8 inb(u16 port);

void pic_disable(void);
void IRQ_set_mask(u8 IRQline);
void IRQ_clear_mask(u8 IRQline);
u16 read_pit_count(void);
void set_pit_count(u16 count);
void remapPic();
