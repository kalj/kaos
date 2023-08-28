bits 16

hello_world:
    mov di, msg
.loop:
    mov al, [di]
    and al,al
    jz .done
    mov ah, 0eh
    mov bh, 00h
    mov bl, 09h
    int 10h
    inc di
    jmp .loop
.done:
    hlt

.halt:
    jmp .halt

msg:
    db `Hello from kernel.asm\n\r\0`

    
