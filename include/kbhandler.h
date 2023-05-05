#pragma once

#include "typedefs.h"
#include "textprint.h"

#define LSHIFT ((u8)(1))
#define RSHIFT ((u8)(1 << 1))
#define LCTRL ((u8)(1 << 2))
#define RCTRL ((u8)(1 << 3))
#define LALT ((u8)(1 << 4))
#define RALT ((u8)(1 << 5))

extern u8 kbFlag;
extern u8 lastScancode;

void stdKbHandler(u8 scancode, u8 chr);
void kbHandler0xe0(u8 scancode);
void kbHandler(u8 scancode, u8 chr);
