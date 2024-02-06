#pragma once

#include "typedefs.h"
#include "io.h"

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

void pic_disable(void);

/** @brief Returns combined value of the cascaded PICs irq request register */
u16 pic_get_irr(void);

/** @brief Returns the combined value of the cascaded PICs in-service register*/
u16 pic_get_isr(void);

void pic_set_mask(u8 IRQline);
void pic_clear_mask(u8 IRQline);
void pic_remap();

void pic_sendEOI(u8 irq);
