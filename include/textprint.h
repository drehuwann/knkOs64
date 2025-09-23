#pragma once

#include "typedefs.h"
#include "io.h"
#include "textmodecolorcodes.h"
#include "memory.h"
#include "varargs.h"
#include "klog.h"

#define VGAMEMORY (u8 *)0xb8000
#define VGAWIDTH 80
#define VGAHEIGHT 25
#define TABSIZE 4

extern s16 cursorPosition;

void clearScreenC(u8 color);
void clearScreen();
void scrollUp();
void setCursorPosition(s16 pos);
u16 posFromCoords(u8 x, u8 y);
void printCharC(u8 c, u8 color);
void printChar(u8 c);
void printStrC(const u8 *str, u8 color);
void printStr(const u8 *str);

extern u8 strOutput[128];

const u8 *hex2strq(u64 val);
const u8 *hex2strd(u32 val);
const u8 *hex2strw(u16 val);
const u8 *hex2strb(u8 val);
const u8 *hex2str(void *any);
const u8 *int2str(s64 value);
const u8 *double2str(double value, u8 decimalPlaces);
const u8 *float2str(float value, u8 decimalPlaces);
const u8 *bin2str(u64 value, u8 digits);

/** @brief kernel mode printf like utility */
void kprintf(const u8 *str, arg_list *args);
