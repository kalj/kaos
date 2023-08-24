org 0x7c00
bits 16

    jmp hello_world
    nop

    db 'MY OEM   '              ; OEM name
    dw 512                      ; bytes per sector
    db 1                        ; sectors per cluster
    dw 1                        ; reserved sectors
    db 2                        ; num FATs
    dw 224                      ; num rootdir entries
    dw 2880                     ; tot sectors
    db 240                      ; media descriptor
    dw 9                        ; sectors per FAT

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
    db `HELO\n\r\0`
times 510-($-$$) db 0
    dw 0AA55h
