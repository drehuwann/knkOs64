#pragma once

#include "typedefs.h"
#include "io.h"
#include "textmodecolorcodes.h"
#include "memory.h"
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
void printCharC(char c, u8 color);
void printChar(char c);
void printStrC(const char *str, u8 color);
void printStr(const char *str);

extern char hex2strOutput[128];
extern char int2strOutput[128];
extern char float2strOutput[128];

const char *hex2strq(u64 val);
const char *hex2strd(u32 val);
const char *hex2strw(u16 val);
const char *hex2strb(u8 val);
const char *hex2str(void *any);
const char *int2str(s64 value);
const char *float2str(float value, u8 decimalPlaces);
