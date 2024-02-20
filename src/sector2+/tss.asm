tss_start:
    dd 0x00000000           ;reserved
rsp0:
    dq 0x0000000000000000   ;rsp entering ring 0
rsp1:
    dq 0x0000000000000000   ;rsp entering ring 1
rsp2:
    dq 0x0000000000000000   ;rsp entering ring 2
    dq 0x0000000000000000   ;reserved

;The next 7 entries are the "Interrupt stack Table"
;Here we can define stack pointers to use when handling interrupts.
;Which one to use is defined in the Interrupt Descriptor Table.
ist1:
    dq 0x0000000000000000
ist2:
    dq 0x0000000000000000
ist3:
    dq 0x0000000000000000
ist4:
    dq 0x0000000000000000
ist5:
    dq 0x0000000000000000
ist6:
    dq 0x0000000000000000
ist7:
    dq 0x0000000000000000   ;reserved
    dw 0x0000               ;reserved
iomap_ptr:
    dw iomap - tss_start
iomap:
tss_end:

tss_limit equ tss_end - tss_start
tss_base equ tss_start
