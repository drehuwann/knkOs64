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

extern char strOutput[128];

const char *hex2strq(u64 val);
const char *hex2strd(u32 val);
const char *hex2strw(u16 val);
const char *hex2strb(u8 val);
const char *hex2str(void *any);
const char *int2str(s64 value);
const char *double2str(double value, u8 decimalPlaces);
const char *float2str(float value, u8 decimalPlaces);
const char *bin2str(u64 value, u8 digits);

/** @brief kernel mode printf like utility */
void kprintf(const u8 *str, arg_list *args);
