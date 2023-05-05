PROGRAM_SPACE equ 0x8000

ReadDisk:
    mov bx, PROGRAM_SPACE
    mov ah, 0x02            ; base function of int 0x13 : readDisk
    mov al, 0x30            ; Number of sectors to read
    mov dl, [BOOT_DRIVE]
    mov ch, 0x00            ; Cylinder index
    mov dh, 0x00            ; Head index
    mov cl, 0x02            ; read from sector 2

    int 0x13                ; diskread bios interrupt.
    jc ReadDiskErr
    ret

BOOT_DRIVE:
    db 0

ReadDiskErr:
    mov bx, DiskReadErrStr
    call PrintString
    jmp $

DiskReadErrStr:
    db 'DiskRead failed !', 0