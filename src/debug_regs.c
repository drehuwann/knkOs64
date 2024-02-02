#include "debug_regs.h"

static bool debregs_got = false;

void dr_clear(debregs *drs) {
    if (drs) memset((void *)drs, 0, sizeof(debregs));
    debregs_got = false;
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
    debregs_got = true;
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
