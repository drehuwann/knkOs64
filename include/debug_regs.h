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
void dr_print(const debregs *drs);

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
void gr_print(const genregs *grs);

typedef struct ctlregs {
    u64 cr0;
    u64 cr2;
    u64 cr3;
    u64 cr4;
    u64 cr8;
} ctlregs;

/** @brief sets all control regs fields to zero */
void cr_clear(ctlregs *crs);

/** @brief set ctlregs with actual cpu state */
void cr_get(ctlregs *crs);

/** @brief display control registers. */
void cr_print(const ctlregs *crs);

/** @brief trick to get cs:rip in the context of execution 
 * @param cs pointer on a word to hold cs
 * @param rip pointer on a quad to hold rip 
*/
static inline void get_cs_rip(u16 *const cs, u64 *const rip) {
    *cs = 0;
    *rip = 0;
    asm volatile (
        "call 1f\n"            // Call the next instruction
        "1:\n"
        "pop %%rax\n"          // Restore rip in rax
        "movw %%cs, %0\n"      // Move cs to cs variable
        "movq %%rax, %1\n"     // Move the address of the next instruction (RIP) to rip variable
        :"=r"(*cs), "=r"(*rip) // Outputs
        :                      // No inputs
        : "%rax"               // Clobbered register
    );
}

/// @brief Struct to represent the RFLAGS register
typedef struct rflags {
    u32 carry      : 1;  // Carry Flag
    u32 reserved1  : 1;  // Reserved
    u32 parity     : 1;  // Parity Flag
    u32 reserved2  : 1;  // Reserved
    u32 adjust     : 1;  // Adjust Flag
    u32 reserved3  : 1;  // Reserved
    u32 zero       : 1;  // Zero Flag
    u32 sign       : 1;  // Sign Flag
    u32 trap       : 1;  // Trap Flag
    u32 interrupt  : 1;  // Interrupt Enable Flag
    u32 direction  : 1;  // Direction Flag
    u32 overflow   : 1;  // Overflow Flag
    u32 iopl       : 2;  // I/O Privilege Level (bits 12 and 13)
    u32 nested_task: 1;  // Nested Task Flag
    u32 reserved4  : 1;  // Reserved
    u32 resume     : 1;  // Resume Flag
    u32 vif        : 1;  // Virtual Interrupt Flag
    u32 vip        : 1;  // Virtual Interrupt Pending Flag
    u32 reserved5  : 1;  // Reserved
    u32 id         : 1;  // Identification Flag
    u32 padding    : 11; // to align on 32 bits boundary
    u32 reserved   : 32; // to align on 64 bits boundary
    // Function pointers for operations
    u8 (*get_iopl)(const struct rflags*); // Function pointer to get IOPL
    void (*print_flags)(const struct rflags*); // Function pointer to print flags
    int (*check_flags)(const struct rflags*, u64); // Function pointer to check flags
} rflags;

// callbacks-kindof
u8 get_iopl_impl(const rflags*); 
void print_flags_impl(const rflags*); 
int check_flags_impl(const rflags*, u64); 

static inline const rflags get_rflags() {
    rflags rfs;
    u64 raw_flags;
    asm volatile (
        "pushf\n"
        "pop %0\n"
        : "=r" (raw_flags)
    );
    rfs = *(rflags*)&raw_flags; // Cast the raw_flags to rflags struct

    // Set function pointers
    rfs.get_iopl = &get_iopl_impl;
    rfs.print_flags = &print_flags_impl;
    rfs.check_flags = &check_flags_impl;
    return rfs;
}
