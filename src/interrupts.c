#include "interrupts.h"

static const u8 exColor = BACKGROUND_BLACK | FOREGROUND_RED; 
static const u8 intColor = BACKGROUND_BLACK | FOREGROUND_YELLOW; 

void (*mainkbhandler)(u8 scancode, u8 chr);

/// Use this when called from an interrupt handler.
__attribute__((no_caller_saved_registers)) static u8 inb_wrap(u8 port) {
    return inb(port);
}

void exc_dflt_handler(int_frame *frame) {
    printStrC("Default exception handler, no errorcode\n\r", exColor);
}

void exc_dflt_handler_err(int_frame_err *frame) {
    printStrC("Default exception handler, errorcode : 0x", exColor);
    printStrC(hex2strq(frame->errorcode), exColor);
    printStr("\n\r");
}

void int_dflt_handler(int_frame *frame) {
    printStrC("Default interrupt handler\n\r", intColor);
}

void exc00_handler(int_frame *frame) {
//    klog("in 'void exc00_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Division by zero exception catched\n\r", exColor);
    frame->rip ++; //DIV or IDIV opcode size is 1
}

void exc01_handler(int_frame *frame) {
//    klog("in 'void exc01_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    bool isFault = false;
    printStrC("Harware debug exception catched\n\r", exColor);
    debregs drs;
    dr_clear(&drs);
    dr_get(&drs);
    dr_print(&drs);

    // fault if (Bn = 1 and (Gn or Ln = 1) and R/Wn = 0) or (BD = 1)
    isFault = (drs.dr6 & (1 << 13)); //if (BD==1)
    u8 index = 0;
    while ((!isFault) && index < 4) {
        isFault = (
            (drs.dr6 & (1 << index)) &&
            (drs.dr7 & (0b11 << (2 * index))) &&
            (((drs.dr7) >> 16) & (0b11 << (4 * index)) == 0)
        );
        ++ index;
    }

    if (isFault) frame->rflags = frame->rflags | (1 << 16);
    // sets ResumeFlag to avoid reentrance
}

void exc02_handler(int_frame *frame) {
    PUSHALL
//    klog("in 'void exc02_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Non maskable interrupt exception catched\n\r", exColor);
    POPALL
}

void exc03_handler(int_frame *frame) {
//    klog("in 'void exc03_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Software debug exception catched\n\r", exColor);
    debregs drs;
    dr_clear(&drs);
    dr_get(&drs);
    dr_print(&drs);
}

void exc04_handler(int_frame *frame) {
//    klog("in 'void exc04_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Overflow exception catched\n\r", exColor);
}

void exc05_handler(int_frame *frame) { //should not occur in 64bit mode !!
//    klog("in 'void exc05_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("BOUND range exception catched\n\r", exColor);
}

void exc06_handler(int_frame *frame) {
//    klog("in 'void exc06_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Invalid opcode exception catched\n\r", exColor);
}

void exc07_handler(int_frame *frame) {
//    klog("in 'void exc07_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Device unavailable exception catched\n\r", exColor);
}

void exc08_handler(int_frame_err *frame) {
//klog("in 'void exc08_handler(int_frame_err *)'", __FILE__, __LINE__, ERROR);
    printStrC("Double fault exception catched\n\r", exColor);
    genregs grs;
    gr_clear(&grs);
    gr_get(&grs);
    gr_print(&grs);
    asm volatile("hlt");
}

void exc0a_handler(int_frame_err *frame) {
//klog("in 'void exc0a_handler(int_frame_err *)'", __FILE__, __LINE__, ERROR);
    printStrC("Invalid TSS exception catched\n\r", exColor);
}

void exc0b_handler(int_frame_err *frame) {
//    klog("in 'void exc0b_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Segment not present exception catched\n\r", exColor);
}

void exc0c_handler(int_frame_err *frame) {
//    klog("in 'void exc0c_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Stack fault exception catched\n\r", exColor);
}

void exc0d_handler(int_frame_err *frame) {
//klog("in 'void exc0d_handler(int_frame_err *)'", __FILE__, __LINE__, ERROR);
    printStrC("General protection exception catched\n\r", exColor);
    u64 errcode = frame->errorcode;
    printStr("\terrorcode = 0x");
    printStr(hex2strq(errcode));
    printStr("\n\r");
    printStr("\tcs:rip = 0x");
    printStr(hex2strw((u16)(frame->cs)));
    printChar(':');
    printStr(hex2strq(frame->rip));
    printStr("\n\r");
    printStr("\tss:rsp = 0x");
    printStr(hex2strw((u16)(frame->ss)));
    printChar(':');
    printStr(hex2strq(frame->rsp));
    printStr("\n\r");
    printStr("\trflags = 0b");
    printStr(bin2str(frame->rflags, 32));
    printStr("\n\r");
    asm volatile("1:hlt;jmp 1b");
}

void exc0e_handler(int_frame_err *frame) {
    ctlregs crs;
    cr_clear(&crs);
    cr_get(&crs);
//    klog("in 'void exc0e_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Page fault exception catched\n\r", exColor);
    u64 errcode = frame->errorcode;
    printStr("\t(u16)errorcode = 0b");
    printStr(bin2str(errcode, 16));
    printStr("\n\r");
    cr_print(&crs);
    printStr("\tIA32_EFER.LME is ");
    u64 retval = 0;
    asm volatile("pushq %%rcx; pushq %%rax; pushq %%rdx;\
        mov $0xc0000080, %%ecx; rdmsr; movq %%rax, %0; popq %%rdx; popq %%rax;\
        popq %%rcx;":"=r"(retval));
    if (!(retval & (1 << 8))) { // IA-32e mode operation disabled.
        printStr("un");
    }
    printStr("set.\n\r");
    asm volatile("mov %0, %%cr2"::"r"(crs.cr2)); //restore cr2
}

void exc10_handler(int_frame *frame) {
//    klog("in 'void exc10_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("x87 FPU exception catched\n\r", exColor);
}

void exc11_handler(int_frame_err *frame) {
//    klog("in 'void exc11_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Align check exception catched\n\r", exColor);
}

void exc12_handler(int_frame *frame) {
// klog("in 'void exc12_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Machine check exception catched\n\r", exColor);
}

void exc13_handler(int_frame *frame) {
//    klog("in 'void exc13_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("SIMD fpu exception catched\n\r", exColor);
}

void exc14_handler(int_frame *frame) {
//    klog("in 'void exc14_handler(int_frame *)'", __FILE__, __LINE__, ERROR);
    printStrC("Virtualization exception catched\n\r", exColor);
}

void irq00_handler(int_frame *frame) {
    klog("in 'void irq00_handler(int_frame *)'", __FILE__, __LINE__, TRACE);
    Tick();
    pic_sendEOI(PIC1_OFFSET + 0x00);
}

void irq01_handler(int_frame *frame) {
    klog("in 'void irq01_handler(int_frame *)'", __FILE__, __LINE__, TRACE);
    u8 scancode = inb_wrap(0x60);
    u8 chr = 0x00;

    if (scancode < 0x50) chr = scanCodeLookupTable[scancode];
    if (mainkbhandler != 0) mainkbhandler(scancode, chr);

    pic_sendEOI(PIC1_OFFSET + 0x01);
}

void irq04_handler(int_frame *frame) {
    printStrC("COM1 interruption catched\n\r", exColor);
    pic_sendEOI(PIC1_OFFSET + 0x04);
}

void irq08_handler(int_frame *frame) {
    printStrC("RTC interruption catched\n\r", exColor);
    pic_sendEOI(PIC1_OFFSET + 0x08);
}

void irq0c_handler(int_frame *frame) {
    printStrC("PS/2 mouse interruption catched\n\r", exColor);
    pic_sendEOI(PIC1_OFFSET + 0x0c);
}
