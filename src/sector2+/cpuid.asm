detectCPUID:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    jz noCPUID
    ret

detectLongMode:
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz noLongMode
    ret

getMaxPhyAddr:
    mov eax, 0x80000008
    cpuid
    mov [MaxPhyAddr], al
    ret

noLongMode:
    hlt                 ; no long mode support

noCPUID:
    hlt                 ; no cpuid support

MaxPhyAddr:
    db 0
    GLOBAL MaxPhyAddr
