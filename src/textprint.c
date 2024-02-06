#include "textprint.h"

s16 cursorPosition;  //should be signed to handle scrollUp limits

void clearScreenC(u8 color) {
//    klog("in 'void clearScreenC(u8)'", __FILE__, __LINE__, TRACE);
    u64 val = 0;
    val += (u64)color << 8;
    val += (u64)color << 24;
    val += (u64)color << 40;
    val += (u64)color << 56;
    for (u64 *it = (u64 *)VGAMEMORY;
            it < (u64 *)(VGAMEMORY + 2 * VGAWIDTH * VGAHEIGHT); ++it) {
        *it = val;
    }
}

void clearScreen() {
//    klog("in 'void clearScreen()'", __FILE__, __LINE__, TRACE);
    clearScreenC(BACKGROUND_BLACK | FOREGROUND_WHITE);
}

/** @brief scrolls up, line by line, with memcpy */
void scrollUp() {
    u16 *dst = (u16 *)VGAMEMORY;

    //TODO save or send to serial the first line
    for (u8 line = 1; line < VGAHEIGHT; line ++) {
        u16 *src = dst + VGAWIDTH;
        memcpy((void *)dst, (void *)src, 2 * VGAWIDTH);
        dst = src;
    }
    memset((void *)dst, 0, 2 * VGAWIDTH); //clears the bottom line
    setCursorPosition(cursorPosition - 2 * VGAWIDTH); // cursor -> one line up
}

void setCursorPosition(s16 pos) {
//    klog("in 'void setCursorPosition(u16)'", __FILE__, __LINE__, TRACE);
    if (pos < 0) pos = 0;
    if (pos > 1999) scrollUp();
    outb(0x03d4, 0x0f);
    outb(0x03d5, (u8)(pos & 0xff));
    outb(0x03d4, 0x0e);
    outb(0x03d5, (u8)((pos >> 8) & 0xff));
    cursorPosition = pos;
}

u16 posFromCoords(u8 x, u8 y) {
    return y * VGAWIDTH + x;
}

void printCharC(u8 c, u8 color) {
    if (c == 0x00) return;
    u16 index = cursorPosition;
    *(VGAMEMORY + index * 2) = c;
    *(VGAMEMORY + index * 2 + 1) = color;
    ++index;
    setCursorPosition(index);
}

void printChar(u8 c) {
    printCharC(c, BACKGROUND_BLACK | FOREGROUND_WHITE);
}

void printStrC(const u8 *str, u8 color) {
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
                while (index > 1999) {
                    scrollUp();
                    index = cursorPosition;
                }
                *(VGAMEMORY + index * 2) = *pstr;
                *(VGAMEMORY + index * 2 + 1) = color;
                ++index;
        }
        ++pstr;
    }
    cursorPosition = index;
    setCursorPosition(cursorPosition);
}

void printStr(const u8 *str) {
    klog("in 'void printStr(const char *)'", __FILE__, __LINE__, TRACE);
    printStrC(str, BACKGROUND_BLACK | FOREGROUND_WHITE);
}

char strOutput[128];

const char *hex2strq(u64 val) {
    u64 *pval = &val;
    u8 *ptr = 0;
    u8 temp = 0;
    u8 size = sizeof(u64) * 2 - 1;
    u8 i = 0;
    for (i = 0; i < size; i++) {
        ptr = (u8 *)pval + i;
        temp = (*ptr & 0xf0) >> 4;
        strOutput[size - (2 * i + 1)] = temp + (temp > 9 ? 'A' - 10 : '0');
        temp = (*ptr & 0x0f);
        strOutput[size - 2 * i] = temp + (temp > 9 ? 'A' - 10 : '0');
    }
    strOutput[size + 1] = 0x00;
    return strOutput;
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
        strOutput[size - (2 * i + 1)] = temp + (temp > 9 ? 'A' - 10 : '0');
        temp = (*ptr & 0x0f);
        strOutput[size - 2 * i] = temp + (temp > 9 ? 'A' - 10 : '0');
    }
    strOutput[size + 1] = 0x00;
    return strOutput;
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
        strOutput[size - (2 * i + 1)] = temp + (temp > 9 ? 'A' - 10 : '0');
        temp = (*ptr & 0x0f);
        strOutput[size - 2 * i] = temp + (temp > 9 ? 'A' - 10 : '0');
    }
    strOutput[size + 1] = 0x00;
    return strOutput;
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
        strOutput[size - (2 * i + 1)] = temp + (temp > 9 ? 'A' - 10 : '0');
        temp = (*ptr & 0x0f);
        strOutput[size - 2 * i] = temp + (temp > 9 ? 'A' - 10 : '0');
    }
    strOutput[size + 1] = 0x00;
    return strOutput;
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
    if (isNeg) strOutput[0] = '-';
    while (value / 10 > 0) {
        remain = value % 10;
        value /= 10;
        strOutput[size + isNeg - index ++] = remain + 0x30;
    }
    remain = value % 10;
    strOutput[isNeg] = remain + 0x30;
    strOutput[size + 1 + isNeg] = 0x00;
    return strOutput;
}

const char *double2str(double value, u8 decimalPlaces) {
    char *intPtr = (char*)int2str((u64)value);
    char *floatPtr = strOutput;
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
    return strOutput;
}

const char *float2str(float value, u8 decimalPlaces) {
    return double2str((double)value, decimalPlaces);
}

const char *bin2str(u64 value, u8 digits) {
    if (digits > 64) digits = 64;
    strOutput[digits] = 0x00;
    do {
        digits --;
        strOutput[digits] = '0' + (value % 2);
        value /= 2;
    } while(digits != 0x00);
    return strOutput;
}

void kprintf(const u8 *str, arg_list *args) {
    //TODO make this robust with size tests
    u8 c = 0x00;
    while ((c = *str) != 0x00) {
        argument *poppedArg = (argument *)0;
        if (c == '%') {
            c = *(++str);
            switch(c) {
                case '%':
                    printChar(c);
                    break;
                case 0x00: //illegal
                    asm volatile("int $0x0d"); // #GP exception
                    --str; // to close loop on next iteration
                    break;
                case 'c':
                    poppedArg = arg_pop(&args);
                    printChar(*((u8 *)(poppedArg->arg_addr)));
                    break;
                case 's':
                    poppedArg = arg_pop(&args);
                    printStr((const u8 *)(poppedArg->arg_addr));
                    break;
                case 'd':
                    poppedArg = arg_pop(&args);
                    if (!poppedArg) {
                        asm volatile("int $0x0d");
                        break;
                    }
                    s64 value = 0;
                    switch(poppedArg->size) {
                        case 1:
                            value = (s64)(*(s8 *)(poppedArg->arg_addr));
                            break;
                        case 2:
                            value = (s64)(*(s16 *)(poppedArg->arg_addr));
                            break;
                        case 4:
                            value = (s64)(*(s32 *)(poppedArg->arg_addr));
                            break;
                        case 8:
                            value = *(s64 *)(poppedArg->arg_addr);
                            break;
                        default:
                            asm volatile("int $0x0d");
                            break;
                    }
                    printStr(int2str(value));
                    break;
                case 'x':
                    poppedArg = arg_pop(&args);
                    if (!poppedArg) {
                        asm volatile("int $0x0d");
                        break;
                    }
                    printStr("0x");
                    switch(poppedArg->size) {
                        case 1:
                            u8 valb = *(u8 *)(poppedArg->arg_addr);
                            printStr(hex2strb(valb));
                            break;
                        case 2:
                            u16 valw = *(u16 *)(poppedArg->arg_addr);
                            printStr(hex2strw(valw));
                            break;
                        case 4:
                            u32 vald = *(u32 *)(poppedArg->arg_addr);
                            printStr(hex2strd(vald));
                            break;
                        case 8:
                            u64 valq = *(u64 *)(poppedArg->arg_addr);
                            printStr(hex2strq(valq));
                            break;
                        default:
                            asm volatile("int $0x0d");
                            break;
                    }
                case 'p':
                    poppedArg = arg_pop(&args);
                    printStr("@0x");
                    printStr(hex2strq(*((u64*)(poppedArg->arg_addr))));
                    break;
                default: //unhandled case DANGER !!
                    asm volatile("int $0x0d");
                    break;
            }
        } else {
            printChar(c);            
        }
        if (poppedArg) free(poppedArg);
        ++str;
    }
}
