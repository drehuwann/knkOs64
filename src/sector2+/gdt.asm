gdt_nulldesc:
    dd 0x00000000
    dd 0x00000000
gdt_codedesc:
    dw 0xffff           ; limit[0:15]
    dw 0x0000           ; base[0:15]
    db 0x00             ; base[16:23]
    db 10011010b        ; access byte
    db 11001111b        ; flags+limit[16:19]
    db 0x00             ; base[24:31]
gdt_datadesc:
    dw 0xffff           ; limit[0:15]
    dw 0x0000           ; base[0:15]
    db 0x00             ; base[16:23]
    db 10010010b        ; access byte
    db 11001111b        ; flags+limit[16:19]
    db 0x00             ; base[24:31]
gdt_user_codedesc:
    dw 0xffff           ; limit[0:15]
    dw 0x0000           ; base[0:15]
    db 0x00             ; base[16:23]
    db 11111010b        ; access byte
    db 11001111b        ; flags+limit[16:19]
    db 0x00             ; base[24:31]
gdt_user_datadesc:
    dw 0xffff           ; limit[0:15]
    dw 0x0000           ; base[0:15]
    db 0x00             ; base[16:23]
    db 11110010b        ; access byte
    db 11001111b        ; flags+limit[16:19]
    db 0x00             ; base[24:31]
gdt_tss_desc:
    dw 0x0000           ; limit[0:15]
    dw 0x0000           ; base[0:15]
    db 0x00             ; base[16:23]
    db 00000000b        ; access byte
    db 00000000b        ; flags+limit[16:19] (upper limit should be 0)
    db 0x00             ; base[24:31]
gdt_tss_desc_high:
    dd 0x00000000       ; base[32:63]
    dd 0x00000000       ; reserved
gdt_end:

gdt_descriptor:
    gdt_size:
        dw gdt_end - gdt_nulldesc - 1
    gdt_base:
        dq gdt_nulldesc

codeSeg equ gdt_codedesc - gdt_nulldesc
dataSeg equ gdt_datadesc - gdt_nulldesc
userCSeg equ gdt_user_codedesc - gdt_nulldesc
userDSeg equ gdt_user_datadesc - gdt_nulldesc
tssSeg equ gdt_tss_desc - gdt_nulldesc

[BITS 32]
EditGDT:
    mov [gdt_codedesc + 6], byte 10101111b
    mov [gdt_datadesc + 6], byte 10101111b
    mov [gdt_user_codedesc + 6], byte 10101111b
    mov [gdt_user_datadesc + 6], byte 10101111b
    ret
[BITS 16]
