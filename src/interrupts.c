#include "interrupts.h"
#include "debug_regs.h"

static const u8 exColor = BACKGROUND_BLACK | FOREGROUND_RED; 
static const u8 intColor = BACKGROUND_BLACK | FOREGROUND_YELLOW; 

void (*mainkbhandler)(u8 scancode, u8 chr);

/// Use this when called from an interrupt handler.
__attribute__((no_caller_saved_registers)) static u8 inb_wrap(u8 port) {
    return inb(port);
}

void exc_dflt_handler(const int_frame */*frame*/) {
    printStrC("Default exception handler, no errorcode\n\r", exColor);
}

void exc_dflt_handler_err(const int_frame_err *frame) {
    printStrC("Default exception handler, errorcode : 0x", exColor);
    printStrC(hex2strq(frame->errorcode), exColor);
    printStr("\n\r");
}

void int_dflt_handler(const int_frame */*frame*/) {
    printStrC("Default interrupt handler\n\r", intColor);
}

void exc00_handler(int_frame *frame) {
    printStrC("Division by zero exception catched\n\r", exColor);
    frame->rip ++; //DIV or IDIV opcode size is 1
}

void exc01_handler(int_frame *frame) {
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

void exc02_handler(const int_frame */*frame*/) {
    PUSHALL
    printStrC("Non maskable interrupt exception catched\n\r", exColor);
    POPALL
}

void exc03_handler(const int_frame *frame) {
    printStrC("Software debug exception catched\n\r", exColor);
    debregs drs;
    dr_clear(&drs);
    dr_get(&drs);
    dr_print(&drs);
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
    printStr("\tIA32_EFER.LME is ");
    u64 retval = 0;
    asm volatile("pushq %%rcx; pushq %%rax; pushq %%rdx; mov $0xc0000080, %%ecx; "
        "rdmsr; movq %%rax, %0; popq %%rdx; popq %%rax; popq %%rcx;"
        :"=r"(retval)
    );
    if (!(retval & (1 << 8))) { // IA-32e mode operation disabled.
        printStr("un");
    }
    printStr("set.\n\r");
    genregs grs;
    gr_clear(&grs);
    gr_get(&grs);
    gr_print(&grs);
}

void exc04_handler(const int_frame */*frame*/) {
    printStrC("Overflow exception catched\n\r", exColor);
}

void exc05_handler(const int_frame */*frame*/) { //should not occur in 64bit mode !!
    printStrC("BOUND range exception catched\n\r", exColor);
}

void exc06_handler(const int_frame *frame) {
    printStrC("Invalid opcode exception catched\n\r", exColor);
    printStr("\tcs:rip = 0x");
    printStr(hex2strw((u16)(frame->cs)));
    printChar(':');
    printStr(hex2strq(frame->rip));
    printStr("\n\r");
    asm volatile("1:hlt;jmp 1b");
}

void exc07_handler(const int_frame */*frame*/) {
    printStrC("Device unavailable exception catched\n\r", exColor);
}

void exc08_handler(const int_frame_err */*frame*/) {
    printStrC("Double fault exception catched\n\r", exColor);
    genregs grs;
    gr_clear(&grs);
    gr_get(&grs);
    gr_print(&grs);
    asm volatile("hlt");
}

void exc0a_handler(const int_frame_err */*frame*/) {
    printStrC("Invalid TSS exception catched\n\r", exColor);
}

void exc0b_handler(const int_frame_err */*frame*/) {
    printStrC("Segment not present exception catched\n\r", exColor);
}

void exc0c_handler(const int_frame_err */*frame*/) {
    printStrC("Stack fault exception catched\n\r", exColor);
}

void exc0d_handler(const int_frame_err *frame) {
    u64 rsp = 0;
    asm volatile("movq %%rsp, %0":"=r"(rsp));
    const rflags rflag = get_rflags();
    u64 rip = 0;
    u16 cs = 0;
    get_cs_rip(&cs, &rip);
    u16 ss = 0;
    asm volatile("movw %%ss, %0":"=r"(ss));
    printStrC("General protection exception catched\r\n", exColor);
    u64 errcode = frame->errorcode;
    if (errcode) {
        printStr("\terrorcode = 0x");
        printStr(hex2strq(errcode));
        printStr("\n\r");
    }
    printStr("\t\t\t\tHANDLER\t\t\t\t|\t\t\t\tFAULT\r\n");
    printStr("cs:rip = 0x");
    printStr(hex2strw(cs));
    printStr(": 0x");
    printStr(hex2strq(rip));
    printStr("\t| 0x");
    printStr(hex2strw((u16)(frame->cs)));
    printStr(": 0x");
    printStr(hex2strq(frame->rip));
    printStr("\r\nss:rsp = 0x");
    printStr(hex2strw(ss));
    printStr(": 0x");
    printStr(hex2strq(rsp));
    printStr("\t| 0x");
    printStr(hex2strw((u16)(frame->ss)));
    printStr(": 0x");
    printStr(hex2strq(frame->rsp));
    printStr("\r\nHNDLR ");
    print_flags_impl(& rflag);
    printStr("FAULT ");
    print_flags_impl((const rflags *)(frame->rflags));
    asm volatile("1:hlt;jmp 1b");
}

void exc0e_handler(const int_frame_err *frame) {
    ctlregs crs;
    cr_clear(&crs);
    cr_get(&crs);
    printStrC("Page fault exception catched\n\r", exColor);
    //TODO if (CPL <= CPL) {if (pageList.isFull()) pageList.freeLeastUsed(); allocateAndPutOnList(page); restore cr2 an iret;}
    u64 errcode = frame->errorcode;
    printStr("\t(u16)errorcode = 0b");
    printStr(bin2str(errcode, 16));
    printStr("\n\r");
    cr_print(&crs);
    printStr("\tIA32_EFER.LME is ");
    u64 retval = 0;
    asm volatile("pushq %%rcx; pushq %%rax; pushq %%rdx; mov $0xc0000080, %%ecx; "
        "rdmsr; movq %%rax, %0; popq %%rdx; popq %%rax; popq %%rcx;"
        :"=r"(retval)
    );
    if (!(retval & (1 << 8))) { // IA-32e mode operation disabled.
        printStr("un");
    }
    printStr("set.\n\r");
    printStr("\tIA32_EFER.NXE is ");
    if (!(retval & (1 << 11))) { //  execute_disabled access rughts.
        printStr("un");
    }
    printStr("set.\n\r");
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
    asm volatile("mov %0, %%cr2"::"r"(crs.cr2)); //restore cr2
}

void exc10_handler(const int_frame */*frame*/) {
    printStrC("x87 FPU exception catched\n\r", exColor);
}

void exc11_handler(const int_frame_err */*frame*/) {
    printStrC("Align check exception catched\n\r", exColor);
}

void exc12_handler(const int_frame */*frame*/) {
    printStrC("Machine check exception catched\n\r", exColor);
}

void exc13_handler(const int_frame */*frame*/) {
    printStrC("SIMD fpu exception catched\n\r", exColor);
}

void exc14_handler(const int_frame */*frame*/) {
    printStrC("Virtualization exception catched\n\r", exColor);
}

void irq00_handler(const int_frame */*frame*/) {
    klog("in 'void irq00_handler(int_frame *)'", __FILE__, __LINE__, TRACE);
    Tick();
    pic_sendEOI(PIC1_OFFSET + 0x00);
}

void irq01_handler(const int_frame */*frame*/) {
    klog("in 'void irq01_handler(int_frame *)'", __FILE__, __LINE__, TRACE);
    u8 scancode = inb_wrap(0x60);
    u8 chr = 0x00;

    if (scancode < 0x50) chr = scanCodeLookupTable[scancode];
    if (mainkbhandler != 0) mainkbhandler(scancode, chr);

    pic_sendEOI(PIC1_OFFSET + 0x01);
}

void irq04_handler(const int_frame */*frame*/) {
    printStrC("COM1 interruption catched\n\r", exColor);
    pic_sendEOI(PIC1_OFFSET + 0x04);
}

void irq08_handler(const int_frame */*frame*/) {
    printStrC("RTC interruption catched\n\r", exColor);
    pic_sendEOI(PIC1_OFFSET + 0x08);
}

void irq0c_handler(const int_frame */*frame*/) {
    printStrC("PS/2 mouse interruption catched\n\r", exColor);
    pic_sendEOI(PIC1_OFFSET + 0x0c);
}
