#pragma once

#include "typedefs.h"
#include "memory.h"
#include "textprint.h"

typedef struct debregs {
    u64 bp_addrs[4];    // dr0..3
    u64 dr6;            // will be dr4 if cr4:DE is cleared
    u64 dr7;            // will be dr5 if cr4:DE is cleared
} debregs;

/** @brief sets all debreg fields to zero */
void dr_clear(debregs *drs);

/** @brief checks if Debug Extensions is present, and sets debregs accordingly*/
void dr_get(debregs *drs);

/** @brief display debug registers. dr[0..3] not displayed if null */
void dr_print(debregs *drs);

typedef struct genregs {
    u64 rax;
    u64 rbx;
    u64 rcx;
    u64 rdx;
    u64 rsi;
    u64 rdi;
    u64 r8;
    u64 r9;
    u64 r10;
    u64 r11;
    u64 r12;
    u64 r13;
    u64 r14;
    u64 r15;
} genregs;

/** @brief sets all genregs fields to zero */
void gr_clear(genregs *grs);

/** @brief set genregs with actual cpu state */
void gr_get(genregs *grs);

/** @brief display general registers. */
void gr_print(genregs *grs);

typedef struct ctlregs {
    u64 cr0;
    u64 cr2;
    u64 cr3;
    u64 cr4;
    u64 cr8;
} ctlregs;

/** @brief sets all control regs fields to zero */
void cr_clear(ctlregs *crs);

/** @brief set genregs with actual cpu state */
void cr_get(ctlregs *crs);

/** @brief display general registers. */
void cr_print(ctlregs *crs);
