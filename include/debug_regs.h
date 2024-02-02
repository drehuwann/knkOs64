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
void dr_clear(debregs *debregs);

/** @brief checks if Debug Extensions is present, and sets debregs accordingly*/
void dr_get(debregs *drs);

/** @brief display debug registers. dr[0..3] not displayed if null */
void dr_print(debregs *drs);
