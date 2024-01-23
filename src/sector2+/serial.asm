;read infos in DBA (in low mem bios 0x400) to list serial ports available
;copies bios detected COM ports into globals com[1..4]
detectCOM:
    push cx
    push si
    push di
    
    xor cx, cx
    mov si, [bda]
    lea di, [com1]
    cld
    mov cx, 4
    rep movsw 
        
    pop di
    pop si
    pop cx

    ret
    
bda:
    dw 0x400

com1:
    dw 0
    GLOBAL com1
com2:
    dw 0
    GLOBAL com2
com3:
    dw 0
    GLOBAL com3
com4:
    dw 0
    GLOBAL com4
