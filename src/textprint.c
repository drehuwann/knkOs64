#include "textprint.h"

u16 cursorPosition;

void clearScreenC(u8 color) {
    klog("in 'void clearScreenC(u8)'", __FILE__, __LINE__, TRACE);
    u64 val = 0;
    val += (u64)color << 8;
    val += (u64)color << 24;
    val += (u64)color << 40;
    val += (u64)color << 56;
    for (u64 *it = (u64 *)VGAMEMORY; it < (u64 *)(VGAMEMORY + 4000); ++it) {
        *it = val;
    }
}

void clearScreen() {
    klog("in 'void clearScreen()'", __FILE__, __LINE__, TRACE);
    clearScreenC(BACKGROUND_BLACK | FOREGROUND_WHITE);
}

void setCursorPosition(u16 pos) {
    klog("in 'void setCursorPosition(u16)'", __FILE__, __LINE__, TRACE);
    if (pos > 1999) pos = 1999;
    outb(0x03d4, 0x0f);
    outb(0x03d5, (u8)(pos & 0xff));
    outb(0x03d4, 0x0e);
    outb(0x03d5, (u8)((pos >> 8) & 0xff));
    cursorPosition = pos;
}

u16 posFromCoords(u8 x, u8 y) {
    return y * VGAWIDTH + x;
}

void printCharC(char c, u8 color) {
    if (c == 0x00) return;
    u16 index = cursorPosition;
    *(VGAMEMORY + index * 2) = c;
    *(VGAMEMORY + index * 2 + 1) = color;
    ++index;
    setCursorPosition(index);
}

void printChar(char c) {
    printCharC(c, BACKGROUND_BLACK | FOREGROUND_WHITE);
}

void printStrC(const char *str, u8 color) {
    u8 *pstr = (u8 *)str;
    u16 index = cursorPosition;
    while (*pstr != 0) {
        switch(*pstr) {
            case 0x09:
                index += TABSIZE;
                break;
            case 0x0a:
                index += VGAWIDTH;
                break;
            case 0x0d:
                index -= index % VGAWIDTH;
                break;
            default:
                *(VGAMEMORY + index * 2) = *pstr;
                *(VGAMEMORY + index * 2 + 1) = color;
                ++index;
        }
        ++pstr;
    }
    cursorPosition = index;
    setCursorPosition(cursorPosition);
}

void printStr(const char *str) {
    klog("in 'void printStr(const char *)'", __FILE__, __LINE__, TRACE);
    printStrC(str, BACKGROUND_BLACK | FOREGROUND_WHITE);
}

char hex2strOutput[128];
char int2strOutput[128];
char float2strOutput[128];

const char *hex2strq(u64 val) {
    u64 *pval = &val;
    u8 *ptr = 0;
    u8 temp = 0;
    u8 size = sizeof(u64) * 2 - 1;
    u8 i = 0;
    for (i = 0; i < size; i++) {
        ptr = (u8 *)pval + i;
        temp = (*ptr & 0xf0) >> 4;
        hex2strOutput[size - (2 * i + 1)] = temp + (temp > 9 ? 'A' - 10 : '0');
        temp = (*ptr & 0x0f);
        hex2strOutput[size - 2 * i] = temp + (temp > 9 ? 'A' - 10 : '0');
    }
    hex2strOutput[size + 1] = 0x00;
    return hex2strOutput;
}

const char *hex2strd(u32 val) {
    u32 *pval = &val;
    u8 *ptr = 0;
    u8 temp = 0;
    u8 size = sizeof(u32) * 2 - 1;
    u8 i = 0;
    for (i = 0; i < size; i++) {
        ptr = (u8 *)pval + i;
        temp = (*ptr & 0xf0) >> 4;
        hex2strOutput[size - (2 * i + 1)] = temp + (temp > 9 ? 'A' - 10 : '0');
        temp = (*ptr & 0x0f);
        hex2strOutput[size - 2 * i] = temp + (temp > 9 ? 'A' - 10 : '0');
    }
    hex2strOutput[size + 1] = 0x00;
    return hex2strOutput;
}

const char *hex2strw(u16 val) {
    u16 *pval = &val;
    u8 *ptr = 0;
    u8 temp = 0;
    u8 size = sizeof(u16) * 2 - 1;
    u8 i = 0;
    for (i = 0; i < size; i++) {
        ptr = (u8 *)pval + i;
        temp = (*ptr & 0xf0) >> 4;
        hex2strOutput[size - (2 * i + 1)] = temp + (temp > 9 ? 'A' - 10 : '0');
        temp = (*ptr & 0x0f);
        hex2strOutput[size - 2 * i] = temp + (temp > 9 ? 'A' - 10 : '0');
    }
    hex2strOutput[size + 1] = 0x00;
    return hex2strOutput;
}

const char *hex2strb(u8 val) {
    u8 *pval = &val;
    u8 *ptr = 0;
    u8 temp = 0;
    u8 size = sizeof(u8) * 2 - 1;
    u8 i = 0;
    for (i = 0; i < size; i++) {
        ptr = (u8 *)pval + i;
        temp = (*ptr & 0xf0) >> 4;
        hex2strOutput[size - (2 * i + 1)] = temp + (temp > 9 ? 'A' - 10 : '0');
        temp = (*ptr & 0x0f);
        hex2strOutput[size - 2 * i] = temp + (temp > 9 ? 'A' - 10 : '0');
    }
    hex2strOutput[size + 1] = 0x00;
    return hex2strOutput;
}

const char *hex2str(void *any) {
    return hex2strq((u64)any);
}

const char *int2str(s64 value) {
    u8 isNeg = (value < 0);
    u8 size = 0;
    if (isNeg) value *= -1;
    u64 sizetester = value;
    while (sizetester / 10 > 0) {
        sizetester /= 10;
        size ++;
    }
    u8 remain, index = 0;
    if (isNeg) int2strOutput[0] = '-';
    while (value / 10 > 0) {
        remain = value % 10;
        value /= 10;
        int2strOutput[size + isNeg - index ++] = remain + 0x30;
    }
    remain = value % 10;
    int2strOutput[isNeg] = remain + 0x30;
    int2strOutput[size + 1 + isNeg] = 0x00;
    return int2strOutput;
}

const char *float2str(float value, u8 decimalPlaces) {
    char *intPtr = (char*)int2str((u64)value);
    char *floatPtr = float2strOutput;
    while (*intPtr != 0x00) {
        *floatPtr ++ = *intPtr ++;
    }
    *floatPtr ++ = '.';
    if (value < 0) value *= -1;
    value -= (int)value;
    for(u8 i = 0; i < decimalPlaces; ++i) {
        value *= 10;
        u8 digit = (int)value;
        value -= digit;
        *floatPtr ++ = digit + 0x30;
    }
    *floatPtr = 0x00;
    return float2strOutput;
}
