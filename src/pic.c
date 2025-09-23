#include "pic.h"

/** @brief Helper func */
static u16 __pic_get_irq_reg(int ocw3) {
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_CMD, (u8)(ocw3 & 0xff));
    outb(PIC2_CMD, (u8)(ocw3 & 0xff));
    return (inb((u16)(PIC2_CMD << 8)) | inb((u16)PIC1_CMD));
}

void pic_disable(void) {
    outb(PIC1_DAT, 0xff);
    outb(PIC2_DAT, 0xff);
}

void pic_set_mask(u8 IRQline) {
    u16 port;
    u8 value;

    if (IRQline < 8) {
        port = PIC1_DAT;
    } else {
        port = PIC2_DAT;
        IRQline -= 8;
    }
    value = inb(port) | (u8)(1 << IRQline);
    outb(port, value);
}

void pic_clear_mask(u8 IRQline) {
    u16 port;
    u8 value;

    if (IRQline < 8) {
        port = PIC1_DAT;
    } else {
        port = PIC2_DAT;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);
}

u16 pic_get_irr(void) {
    return __pic_get_irq_reg(PIC_READ_IRR);
}

u16 pic_get_isr(void) {
    return __pic_get_irq_reg(PIC_READ_ISR);
}

void pic_remap() {
	u8 a1;
    	u8 a2;
	a1 = inb(PIC1_DAT);                     // save masks
	a2 = inb(PIC2_DAT);

    // starts the initialization sequence (in cascade mode)
	outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
	outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DAT, PIC1_OFFSET);          // ICW2: Master PIC vector offset
	outb(PIC2_DAT, PIC1_OFFSET + 8);           // ICW2: Slave PIC vector offset

    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC1_DAT, 4);

    // ICW3: tell Slave PIC its cascade identity (0000 0010)
	outb(PIC2_DAT, 2);

    // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	outb(PIC1_DAT, ICW4_8086);
	outb(PIC2_DAT, ICW4_8086);

	outb(PIC1_DAT, a1);                     // restore saved masks.
	outb(PIC2_DAT, a2);
}

void pic_sendEOI(u8 irq) {
    if(irq >= 8) outb(PIC2_CMD, PIC_EOI);
	outb(PIC1_CMD, PIC_EOI);
}
