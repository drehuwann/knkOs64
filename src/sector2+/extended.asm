jmp enterProtectedMode

%include "gdt.asm"
%include "detectmem.asm"
%include "serial.asm"

enterProtectedMode:
    call detectCOM
    call detectMem
    call enableA20
    cli
    lgdt [gdt_descriptor]
    push eax
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    pop eax
    jmp codeSeg:startProtMode

enableA20:
    in al, 0x92
    or al, 0x02
    out 0x92, al
    ret

[BITS 32]
%include "cpuid.asm"
%include "paging.asm"

startProtMode:
    mov ax, dataSeg
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call detectCPUID
    call detectLongMode
    call setupIdentityPaging
    call EditGDT
    jmp codeSeg:enter64bits

[BITS 64]
[extern _start]

enter64bits:
    mov rdi, 0xb8000
    mov rax, 0x2a202a202a202a20
    mov ecx, 500
    rep stosq
    call activateSSE
    call _start
    jmp $

activateSSE:
    mov rax, cr0
    and ax, 0b11111101
    or ax, 0b00000001
    mov cr0, rax
    mov rax, cr4
    or eax, 0b1100000000
    mov cr4, rax
    ret

times 2048-($-$$) db 0
