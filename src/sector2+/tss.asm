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

extern test_user_function   ; declared/definite in ring3.[h,c]

jump_usermode:  ;enable system call extensions that enables sysret and syscall
    mov rcx, 0xc0000080 ; IA32_EFER msr
	rdmsr
	or eax, 1           ; enable SYSCALL/SYSRET
	wrmsr
	mov rcx, 0xc0000081 ; IA32_STAR msr
	rdmsr
	mov edx, 0x00130008 ; (base kernel/user / (kernel CS) 
	wrmsr
    mov rcx, 0xc0000084 ; IA32_FMASK
    mov edx, 0xffffffff
    mov eax, 0xfffffffd
    wrmsr
	mov rcx, test_user_function ; to be loaded into RIP
	mov r11, 0x202              ; to be loaded into EFLAGS. only IF is set.
    mov [rsp0], rsp             ; save kernel stack pointer
	o64 sysret                  ; sysretq

GLOBAL jump_usermode