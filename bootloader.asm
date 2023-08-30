org 0x7c00+62
bits 16

    ;; Skipping this code here:

    ;; jmp main
    ;; nop


    ;; db 'MY OEM  '               ; OEM name
    ;; dw 512                      ; bytes per sector
    ;; db 1                        ; sectors per cluster
    ;; dw 1                        ; reserved sectors
    ;; db 2                        ; num FATs
    ;; dw 224                      ; num rootdir entries (224 for floppies, 512 for hard drives)
    ;; dw 2880                     ; tot logical sectors
    ;; db 0xf0                     ; media descriptor (f0h for floppies, f8h for hard drives)
    ;; dw 9                        ; logical sectors per FAT

    ;; dw 18                       ; sectors per track
    ;; dw 2                        ; number of heads
    ;; dd 0                        ; hidden sectors
    ;; dd 0                        ; large tot log sectors

    ;; db 0                        ; physical drive number
    ;; db 0                        ; flags etc
    ;; db 0x29                     ; extended boot sign
    ;; dd 0x00001234               ; volume serial num
    ;; db 'BIRGER     '            ; volume label
    ;; db 'FAT12   '            ; filesystem type

main:

    ;; mov al,                     ; total sector count
    ;; mov ch                      ; low 8 bits of cylinder number
    ;; mov cl                      ; sector (bits 0-5), high 2 bits of cylinder num (bits 6-7)
    ;; mov dh                      ; head number
    ;; mov dl                      ; drive number
    ;; mov ah, 2                   ; do read
    mov ah, 8
    mov dl, 0
    mov di, 0
    mov es, di
    int 13h
    ;; bl drive type                  
    ;; ch low 8 bits of max cylinder num
    ;; cl max sector num (5-0), high two bits of max cylinder num (7-6)
    ;; dh maximum head number
    ;; dl numnber of drives
    ;; es:di drive parameter table
    push bx
    push cx
    push dx
    push es
    push di

    ;; drive type: 4 ->2.88M floppy
    ;; heads: 2
    ;; drives: 1
    ;; max cylinder num: 0b00:0x4f  79 -> 80 cylinders/tracks
    ;; max sector num: 0x12 = 18 -> 18 sectors

    ;; print di
    pop di
    call print_hex16
    mov di, line_end_str
    call print_str

    ;; print es
    pop di
    call print_hex16
    mov di, line_end_str
    call print_str

    ;; print dx
    pop di
    call print_hex16
    mov di, line_end_str
    call print_str

    ;; print cx
    pop di
    call print_hex16
    mov di, line_end_str
    call print_str

    ;; print bx
    pop di
    call print_hex16
    mov di, line_end_str
    call print_str

    ;; mov di, msg
    ;; call print_str
    jmp the_end


print_hex16:  
    mov bx, 4

.print_nibble:
    xor al,al
    mov cx,4
.loop:
    shl di, 1
    rcl al, 1
    dec cx
    jnz .loop

    cmp al, 9
    jg .alpha
    add al, '0'
    jmp .hexification_done

.alpha:    
    sub al, 10
    add al, 'a'
.hexification_done:
    push bx
    ;; al is byte
    mov ah, 0eh
    mov bh, 00h
    mov bl, 09h
    int 10h

    pop bx
    dec bx
    jnz .print_nibble

    ret

print_char: 
    mov ah, 0eh
    mov bh, 00h
    mov bl, 09h
    int 10h
    ret


print_str:  
.loop:
    mov al, [di]
    and al,al
    jz .done
    call print_char
    inc di
    jmp .loop
.done:
    ret

the_end:
    hlt
.halt:
    jmp .halt

msg:
    db "banan",0
line_end_str:
    db `\r\n\0`

kernel_filename:
    db "kernel.bin"
;; times 510-($-$$) db 0
;;     dw 0AA55h
