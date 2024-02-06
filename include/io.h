#pragma once

#include "typedefs.h"
// Don't include klog.h there (reentrance if klog used with outb() !)

void outb(u16 port, u8 val);
u8 inb(u16 port);
