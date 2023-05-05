PrintString:
    push ax
    push bx
    mov ah, 0x0e
    .Begin:
        cmp [bx], byte 0
        je .Exit
        mov al, [bx]
        int 0x10
        inc bx
        jmp .Begin
    .Exit:
        mov al, 0x0d        ; Carriage Return
        int 0x10
        mov al, 0x0a        ; Line Feed
        int 0x10
    pop bx
    pop ax
    ret

HelloString:
    db "Hello booted knkOs64", 0
