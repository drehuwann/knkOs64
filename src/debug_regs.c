#include "debug_regs.h"

void dr_clear(debregs *drs) {
    if (drs) memset((void *)drs, 0, sizeof(debregs));
}

void dr_get(debregs *drs) {
    u64 retval = 0;
    asm volatile("mov %%db0, %0":"=r"(retval));
    drs->bp_addrs[0] = retval;
    retval = 0;
    asm volatile("mov %%db1, %0":"=r"(retval));
    drs->bp_addrs[1] = retval;
    retval = 0;
    asm volatile("mov %%db2, %0":"=r"(retval));
    drs->bp_addrs[2] = retval;
    retval = 0;
    asm volatile("mov %%db3, %0":"=r"(retval));
    drs->bp_addrs[3] = retval;
    retval = 0;
    asm volatile("mov %%cr4, %0":"=r"(retval));
    bool has_debug_xtension = ((retval & 0b1000) != 0);
    retval = 0;
    if (has_debug_xtension) {
        asm volatile ("mov %%db6, %0":"=r"(retval));
        drs->dr6 = retval;
        retval = 0;
        asm volatile ("mov %%db7, %0":"=r"(retval));
        drs->dr7 = retval;
    } else {
        asm volatile ("mov %%db4, %0":"=r"(retval));
        drs->dr6 = retval;
        retval = 0;
        asm volatile ("mov %%db5, %0":"=r"(retval));
        drs->dr7 = retval;
    }
}

void dr_print(debregs const *drs) {
    u64 addr = 0;
    for (u8 i = 0; i < 4; i ++) {
        if ((addr = (drs->bp_addrs)[i]) != 0) {
            printStr((const u8 *)"\t#DB");
            printStr(int2str(i));
            printStr((const u8 *)" @ 0x");
            printStr(hex2strq(addr));
            printStr((const u8 *)"\n\r");
        }
    }
    addr = drs->dr6;
    printStr((const u8 *)"\t#DB6 = 0x");
    printStr(hex2strq(addr));
    printStr((const u8 *)"\n\r");
    addr = drs->dr7;
    printStr((const u8 *)"\t#DB7 = 0x");
    printStr(hex2strq(addr));
    printStr((const u8 *)"\n\r");
}

void gr_clear(genregs *grs) {
    if (grs) memset((void *)grs, 0, sizeof(genregs));
}

void gr_get(genregs *grs) {
    u64 retval = 0;
    asm volatile("mov %%rax, %0":"=r"(retval));
    grs->rax = retval;
    retval = 0;
    asm volatile("mov %%rbx, %0":"=r"(retval));
    grs->rbx = retval;
    retval = 0;
    asm volatile("mov %%rcx, %0":"=r"(retval));
    grs->rcx = retval;
    retval = 0;
    asm volatile("mov %%rdx, %0":"=r"(retval));
    grs->rdx = retval;
    retval = 0;
    asm volatile("mov %%rsi, %0":"=r"(retval));
    grs->rsi = retval;
    retval = 0;
    asm volatile("mov %%rdi, %0":"=r"(retval));
    grs->rdi = retval;
    retval = 0;
    asm volatile("mov %%r8, %0":"=r"(retval));
    grs->r8 = retval;
    retval = 0;
    asm volatile("mov %%r9, %0":"=r"(retval));
    grs->r9 = retval;
    retval = 0;
    asm volatile("mov %%r10, %0":"=r"(retval));
    grs->r10 = retval;
    retval = 0;
    asm volatile("mov %%r11, %0":"=r"(retval));
    grs->r11 = retval;
    retval = 0;
    asm volatile("mov %%r12, %0":"=r"(retval));
    grs->r12 = retval;
    retval = 0;
    asm volatile("mov %%r13, %0":"=r"(retval));
    grs->r13 = retval;
    retval = 0;
    asm volatile("mov %%r14, %0":"=r"(retval));
    grs->r14 = retval;
    retval = 0;
    asm volatile("mov %%r15, %0":"=r"(retval));
    grs->r15 = retval;
}

void gr_print(const genregs *grs) {
    u64 addr = grs->rax;
    printStr((const u8 *)"\trax = 0x");
    printStr(hex2strq(addr));
    addr = grs->rbx;
    printStr((const u8 *)"\trbx = 0x");
    printStr(hex2strq(addr));
    printStr((const u8 *)"\n\r");
    addr = grs->rcx;
    printStr((const u8 *)"\trcx = 0x");
    printStr(hex2strq(addr));
    addr = grs->rdx;
    printStr((const u8 *)"\trdx = 0x");
    printStr(hex2strq(addr));
    printStr((const u8 *)"\n\r");
    addr = grs->r8;
    printStr((const u8 *)"\tr8  = 0x");
    printStr(hex2strq(addr));
    addr = grs->r9;
    printStr((const u8 *)"\tr9  = 0x");
    printStr(hex2strq(addr));
    printStr((const u8 *)"\n\r");
    addr = grs->r10;
    printStr((const u8 *)"\tr10 = 0x");
    printStr(hex2strq(addr));
    addr = grs->r11;
    printStr((const u8 *)"\tr11 = 0x");
    printStr(hex2strq(addr));
    printStr((const u8 *)"\n\r");
    addr = grs->r12;
    printStr((const u8 *)"\tr12 = 0x");
    printStr(hex2strq(addr));
    addr = grs->r13;
    printStr((const u8 *)"\tr13 = 0x");
    printStr(hex2strq(addr));
    printStr((const u8 *)"\n\r");
    addr = grs->r14;
    printStr((const u8 *)"\tr14 = 0x");
    printStr(hex2strq(addr));
    addr = grs->r15;
    printStr((const u8 *)"\tr15 = 0x");
    printStr(hex2strq(addr));
    printStr((const u8 *)"\n\r");
}

void cr_clear(ctlregs *crs) {
    if (crs) memset((void *)crs, 0, sizeof(ctlregs));
}

void cr_get(ctlregs *crs) {
    u64 retval = 0;
    asm volatile("mov %%cr0, %0":"=r"(retval));
    crs->cr0 = retval;
    retval = 0;
    asm volatile("mov %%cr2, %0":"=r"(retval));
    crs->cr2 = retval;
    retval = 0;
    asm volatile("mov %%cr3, %0":"=r"(retval));
    crs->cr3 = retval;
    retval = 0;
    asm volatile("mov %%cr4, %0":"=r"(retval));
    crs->cr4 = retval;
    retval = 0;
    asm volatile("mov %%cr8, %0":"=r"(retval));
    crs->cr8 = retval;
}

void cr_print(const ctlregs *crs) {
    u64 addr = crs->cr0;
    printStr((const u8 *)"\tcr0 = 0x");
    printStr(hex2strq(addr));
    addr = crs->cr2;
    printStr((const u8 *)"\tcr2 = 0x");
    printStr(hex2strq(addr));
    printStr((const u8 *)"\n\r");
    addr = crs->cr3;
    printStr((const u8 *)"\tcr3 = 0x");
    printStr(hex2strq(addr));
    addr = crs->cr4;
    printStr((const u8 *)"\tcr4  = 0x");
    printStr(hex2strq(addr));
    printStr((const u8 *)"\n\r");
    addr = crs->cr8;
    printStr((const u8 *)"\tcr8  = 0x");
    printStr(hex2strq(addr));
    printStr((const u8 *)"\n\r");
}


// Function to get IOPL
u8 get_iopl_impl(const rflags *flags) {
    return flags->iopl; // Return the I/O Privilege Level
}

// Function to print set flags
void print_flags_impl(const rflags* flags) {
    printStr((const u8 *)"Flags:");
    if (flags->carry) printStr((const u8 *)" CF");
    if (flags->parity) printStr((const u8 *)" PF");
    if (flags->adjust) printStr((const u8 *)" AF");
    if (flags->zero) printStr((const u8 *)" ZF");
    if (flags->sign) printStr((const u8 *)" SF");
    if (flags->trap) printStr((const u8 *)" TF");
    if (flags->interrupt) printStr((const u8 *)" IF");
    if (flags->direction) printStr((const u8 *)" DF");
    if (flags->overflow) printStr((const u8 *)" OF");
    if (flags->nested_task) printStr((const u8 *)" NT");
    if (flags->vif) printStr((const u8 *)" VIF");
    if (flags->vip) printStr((const u8 *)" VIP");
    if (flags->id) printStr((const u8 *)" ID");
    printStr((const u8 *)" IOPL");
    printStr(int2str(flags->get_iopl(flags)));
    printStr((const u8 *)"\r\n");
}

// Function to check if any flags are set based on a mask
int check_flags_impl(const rflags *flags, u64 mask) {
    return (flags->carry & (mask & 0x1)) ||
           (flags->parity & (mask & 0x4)) ||
           (flags->adjust & (mask & 0x10)) ||
           (flags->zero & (mask & 0x40)) ||
           (flags->sign & (mask & 0x80)) ||
           (flags->trap & (mask & 0x100)) ||
           (flags->interrupt & (mask & 0x200)) ||
           (flags->direction & (mask & 0x400)) ||
           (flags->overflow & (mask & 0x800)) ||
           (flags->nested_task & (mask & 0x2000)) ||
           (flags->vif & (mask & 0x4000)) ||
           (flags->vip & (mask & 0x8000)) ||
           (flags->id & (mask & 0x200000));
}
