memRegionCnt:
    db 0
    GLOBAL memRegionCnt

detectMem:
    xor ax, ax
    mov es, ax
    mov di, 0x5000
    mov edx, 0x534d4150     ; magic 'SMAP'
    xor ebx, ebx
    .re:
        mov eax, 0xe820
        mov ecx, 0x18
        int 0x15
        cmp ebx, 0
        je .finished
        add di, 0x18
        inc byte [memRegionCnt]
        jmp .re
    .finished:
    ret
