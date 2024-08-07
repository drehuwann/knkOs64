PageTableEntry equ 0x1000

setupIdentityPaging:
    mov edi, PageTableEntry
    mov cr3, edi
    mov dword [edi], 0x2007
    add edi, 0x1000
    mov dword [edi], 0x3007
    add edi, 0x1000
    mov dword [edi], 0x4007
    add edi, 0x1000

    mov ebx, 0x00000007
    mov ecx, 512

    .setEntry:
        mov dword [edi], ebx
        add ebx, 0x1000
        add edi, 8
        loop .setEntry

    mov eax, cr4
    or eax, 1 << 5      ;CR4.PAE
    mov cr4, eax

    mov ecx, 0xc0000080 ;IA32_EFER
    rdmsr
    or eax, 1 << 8      ;IA32_EFER.LME
    wrmsr

    mov eax, cr0
    or eax, 1 << 31     ;CR0.PG
    mov cr0, eax

    ret
