org 0x7c00
bits 16

    jmp main
    nop

    db 'MY OEM  '               ; OEM name
    dw 512                      ; bytes per sector
    db 2                        ; sectors per cluster
    dw 1                        ; reserved sectors
    db 2                        ; num FATs
    dw 224                      ; num rootdir entries (224 for floppies, 512 for hard drives)
    dw 5760                     ; tot logical sectors
    db 0xf0                     ; media descriptor (f0h for floppies, f8h for hard drives)
    dw 9                        ; logical sectors per FAT

main:

    mov al,                     ; total sector count
    mov ch                      ; low 8 bits of cylinder number
    mov cl                      ; sector (bits 0-5), high 2 bits of cylinder num (bits 6-7)
    mov dh                      ; head number
    mov dl                      ; drive number
    mov ah, 2                   ; do read
    int 13h
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

kernel_filename:
    db "kernel.bin"
times 510-($-$$) db 0
    dw 0AA55h
