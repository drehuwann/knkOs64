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
    bool has_debug_xtension = (retval & 0b1000 != 0);
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

void dr_print(debregs *drs) {
    u64 addr = 0;
    for (u8 i = 0; i < 4; i ++) {
        if ((addr = (drs->bp_addrs)[i]) != 0) {
            printStr("\t#DB");
            printStr(int2str(i));
            printStr(" @ 0x");
            printStr(hex2strq(addr));
            printStr("\n\r");
        }
    }
    addr = drs->dr6;
    printStr("\t#DB6 = 0x");
    printStr(hex2strq(addr));
    printStr("\n\r");
    addr = drs->dr7;
    printStr("\t#DB7 = 0x");
    printStr(hex2strq(addr));
    printStr("\n\r");
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

void gr_print(genregs *grs) {
    u64 addr = grs->rax;
    printStr("\trax = 0x");
    printStr(hex2strq(addr));
    addr = grs->rbx;
    printStr("\trbx = 0x");
    printStr(hex2strq(addr));
    printStr("\n\r");
    addr = grs->rcx;
    printStr("\trcx = 0x");
    printStr(hex2strq(addr));
    addr = grs->rdx;
    printStr("\trdx = 0x");
    printStr(hex2strq(addr));
    printStr("\n\r");
    addr = grs->r8;
    printStr("\tr8  = 0x");
    printStr(hex2strq(addr));
    addr = grs->r9;
    printStr("\tr9  = 0x");
    printStr(hex2strq(addr));
    printStr("\n\r");
    addr = grs->r10;
    printStr("\tr10 = 0x");
    printStr(hex2strq(addr));
    addr = grs->r11;
    printStr("\tr11 = 0x");
    printStr(hex2strq(addr));
    printStr("\n\r");
    addr = grs->r12;
    printStr("\tr12 = 0x");
    printStr(hex2strq(addr));
    addr = grs->r13;
    printStr("\tr13 = 0x");
    printStr(hex2strq(addr));
    printStr("\n\r");
    addr = grs->r14;
    printStr("\tr14 = 0x");
    printStr(hex2strq(addr));
    addr = grs->r15;
    printStr("\tr15 = 0x");
    printStr(hex2strq(addr));
    printStr("\n\r");
}
