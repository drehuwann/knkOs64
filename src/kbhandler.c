#include "kbhandler.h"
#include "kbscancodeset1.h"

u8 kbFlag = 0;
u8 lastScancode = 0;

void stdKbHandler(u8 scancode, u8 chr) {
    if (chr != 0) {
        if (kbFlag & (LSHIFT | RSHIFT)) {
            if (chr >= 'a' && chr <= 'z') {
                chr -= 0x20;
            } else {
                chr = scanCodeLookupTableShifted[scancode];
            }
        }
        printChar(chr);
    } else {
        switch(scancode) {
            case 0x0e:  //Backspace
                setCursorPosition(cursorPosition - 1);
                printChar(0x20);
                setCursorPosition(cursorPosition - 1);
                break;
            case 0x1c:  //Return
                printStr((const u8 *)"\n\r");
                break;
            case 0x2a:  //Left shift pressed
                kbFlag |= LSHIFT;
                break;
            case 0xaa:  //Left shift released
                kbFlag &= ! LSHIFT;
                break;
            case 0x36:  //Right shift pressed
                kbFlag |= RSHIFT;
                break;
            case 0xb6:  //Right shift released
                kbFlag &= ! RSHIFT;
                break;
            default:
        }
    }
}

void kbHandler0xe0(u8 scancode) {
    switch(scancode) {
        case 0x1c:  //NumPad Return
            printStr((const u8 *)"\n\r");
            break;
        case 0x35:  //NumPad Divide
            printChar('/');
            break;
        case 0x47:  //Home
            setCursorPosition(cursorPosition - (cursorPosition % VGAWIDTH));
            break;
        case 0x48:  //Up
            setCursorPosition(cursorPosition - VGAWIDTH);
            break;
        case 0x49:  //PgUp
            setCursorPosition(0);
            break;
        case 0x4b:  //Left
            setCursorPosition(cursorPosition - 1);
            break;
        case 0x4d:  //Right
            setCursorPosition(cursorPosition + 1);
            break;
        case 0x4f:  //End
            setCursorPosition(
                cursorPosition + VGAWIDTH - cursorPosition % VGAWIDTH - 1);
            break;
        case 0x50:  //Down
            setCursorPosition(cursorPosition + VGAWIDTH);
            break;
        case 0x51:  //PgDn
            setCursorPosition(1999); // no page scrolling for now 
            break;
        default:
    }
}

void kbHandler(u8 scancode, u8 chr) {
    if (lastScancode == 0xe0) {
        kbHandler0xe0(scancode);
    } else {
        stdKbHandler(scancode, chr);
    }
    lastScancode = scancode;
}
