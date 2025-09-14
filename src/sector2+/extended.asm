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
    call getMaxPhyAddr
    call setupIdentityPaging
    call EditGDT
    jmp codeSeg:enter64bits

[BITS 64]
%include "tss.asm"

[extern _start]

enter64bits:
    call preparePageFrameMask
    mov rdi, 0xb8000
    mov rax, 0x2a202a202a202a20
    mov ecx, 500
    rep stosq
    call createTSS
    call initTSS
;    lgdt [gdt_descriptor]       ; reload gdt
    call activateSSE
    call _start
    jmp $

createTSS:
    push rax
    mov rax, tss_limit
    mov [gdt_tss_desc], byte al             ; limit [0:7]
    shr rax, 8
    mov [gdt_tss_desc + 1], byte al         ; limit [8:15]
    mov rax, tss_base
    mov [gdt_tss_desc + 2], byte al         ; base [0:7]
    shr rax, 8
    mov [gdt_tss_desc + 3], byte al         ; base [8:15]
    shr rax, 8
    mov [gdt_tss_desc + 4], byte al         ; base [16:23]
    mov [gdt_tss_desc + 5], byte 10001001b  ; access = 0x89 for TSS
    mov [gdt_tss_desc + 6], byte 00000000b  ; flags = 0x0 for TSS
    shr rax, 8
    mov [gdt_tss_desc + 7], byte al         ; base [24:31]
    shr rax, 8
    mov [gdt_tss_desc_high], byte al        ; base [32:39]
    shr rax, 8
    mov [gdt_tss_desc_high + 1], byte al    ; base [40:47]
    shr rax, 8
    mov [gdt_tss_desc_high + 2], byte al    ; base [48:55]
    shr rax, 8
    mov [gdt_tss_desc_high + 3], byte al    ; base [56:63]
    pop rax
    ret

initTSS:
    push rax
    mov ax, tssSeg | 0x0000 ; symbolically OR-ed with 0 to set the RPL.
    ltr ax
    pop rax
    ret

activateSSE:
    mov rax, cr0
    and ax, 0b11111101
    or ax, 0b00000001
    mov cr0, rax
    mov rax, cr4
    or eax, 0b1100000000
    mov cr4, rax
    ret

preparePageFrameMask:
    xor rax, rax
    mov cl, [MaxPhyAddr]
    sub cl, 0x0c
    .loop:
        shl rax, 1
        inc rax
        dec cl
        jnz .loop
    shl rax, 0x0c
    mov [pageFrameMask], rax
    ret

pageFrameMask:
    dq 0
    GLOBAL pageFrameMask