#include "io.h"

void outb(u16 port, u8 val) {
    // don't klog() there !! (reentrance);
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

u8 inb(u16 port) {
    // don't klog() there !! (reentrance);
    u8 retVal;
    asm volatile ("inb %1, %0" : "=a"(retVal) : "Nd"(port));
    return retVal;
}

void pic_disable(void) {
    outb(PIC1_DAT, 0xff);
    outb(PIC2_DAT, 0xff);
}

void IRQ_set_mask(u8 IRQline) {
    u16 port;
    u8 value;
 
    if (IRQline < 8) {
        port = PIC1_DAT;
    } else {
        port = PIC2_DAT;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
void IRQ_clear_mask(u8 IRQline) {
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

/** @brief Helper func */
static u16 __pic_get_irq_reg(int ocw3) {
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_CMD, ocw3);
    outb(PIC2_CMD, ocw3);
    return (inb(PIC2_CMD) << 8) | inb(PIC1_CMD);
}
 
/** @brief Returns the combined value of the cascaded PICs irq request register
*/
u16 pic_get_irr(void) {
    return __pic_get_irq_reg(PIC_READ_IRR);
}
 
/** @brief Returns the combined value of the cascaded PICs in-service register
*/
u16 pic_get_isr(void) {
    return __pic_get_irq_reg(PIC_READ_ISR);
}

u16 read_pit_count(void) {
	u16 count = 0;
 
	// Disable interrupts
	asm volatile ("cli");
 
	// al = channel in bits 6 and 7, remaining bits clear
	outb(PIT_CMD, 0b0000000);
 
	count = inb(PIT_CH0);		// Low byte
	count |= inb(PIT_CH0) << 8;		// High byte 
	return count;
}

void set_pit_count(u16 count) {
	// Disable interrupts
	asm volatile("cli");
 
	// Set low byte
	outb(PIT_CH0, (u8)(count & 0xff));		// Low byte
	outb(PIT_CH0, (u8)((count & 0xff00) >> 8));	// High byte
}

void remapPic() {
	u8 a1, a2; 
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
