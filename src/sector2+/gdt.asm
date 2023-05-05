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
gdt_end:

gdt_descriptor:
    gdt_size:
        dw gdt_end - gdt_nulldesc - 1
        dq gdt_nulldesc

codeSeg equ gdt_codedesc - gdt_nulldesc
dataSeg equ gdt_datadesc - gdt_nulldesc

[BITS 32]
EditGDT:
    mov [gdt_codedesc + 6], byte 10101111b
    mov [gdt_datadesc + 6], byte 10101111b
    ret
[BITS 16]
