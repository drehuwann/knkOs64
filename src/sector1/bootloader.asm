[ORG 0x7c00]

mov [BOOT_DRIVE], dl

mov bp, 0x7c00
mov sp, bp

mov bx, HelloString
call PrintString

call ReadDisk

jmp PROGRAM_SPACE

%include "print.asm"
%include "diskread.asm"

times 510-($-$$) db 0
dw 0xaa55
