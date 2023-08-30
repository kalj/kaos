org 0x7c00+CODE_START_OFFSET
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

SECTORS_PER_TRACK_ADDR equ (0x7c00+0x18)
    ;; dw 18                       ; sectors per track
NUMBER_OF_HEADS_ADDR equ (0x7c00+0x1a)
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

    mov ax, 1                   ; starting at sector 2
    mov dl, 5                   ; read 5 sectors
    mov bx, 0                   
    mov es, bx
    mov bx, 0x7e00
    call read_sectors

    mov bx,0x7e00

.print_byte:
    mov dl, [bx]
    call print_hex8
    mov di, line_end_str
    call print_str

    inc bx
    cmp bx, 0x7e10
    jl .print_byte

    ;; mov dl, [0x7e01]
    ;; call print_hex8
    ;; mov di, line_end_str
    ;; call print_str

    ;; mov dl, [0x7e02]
    ;; call print_hex8
    ;; mov di, line_end_str
    ;; call print_str

    ;; Read root directory into memory area

    ;; loop over root directory




    ;; mov di, msg
    ;; call print_str

    ;; HALT here
    cli
    hlt
.halt:
    jmp .halt


;; print_bios_disk_params: 
;;     mov ah, 8
;;     mov dl, 0
;;     mov di, 0
;;     mov es, di
;;     int 13h
;;     ;; bl drive type                  
;;     ;; ch low 8 bits of max cylinder num
;;     ;; cl max sector num (5-0), high two bits of max cylinder num (7-6)
;;     ;; dh maximum head number
;;     ;; dl numnber of drives
;;     ;; es:di drive parameter table
;;     push bx
;;     push cx
;;     push dx
;;     push es
;;     push di

;;     ;; drive type: 4 ->2.88M floppy
;;     ;; heads: 2
;;     ;; drives: 1
;;     ;; max cylinder num: 0b00:0x4f  79 -> 80 cylinders/tracks
;;     ;; max sector num: 0x12 = 18 -> 18 sectors

;;     ;; print di
;;     pop di
;;     call print_hex16
;;     mov di, line_end_str
;;     call print_str

;;     ;; print es
;;     pop di
;;     call print_hex16
;;     mov di, line_end_str
;;     call print_str

;;     ;; print dx
;;     pop di
;;     call print_hex16
;;     mov di, line_end_str
;;     call print_str

;;     ;; print cx
;;     pop di
;;     call print_hex16
;;     mov di, line_end_str
;;     call print_str

;;     ;; print bx
;;     pop di
;;     call print_hex16
;;     mov di, line_end_str
;;     call print_str

;;     ret

;; print_hex16:  
;;     args:
;;        di - value to print
;;
;;     mov bx, 4

;; .print_nibble:
;;     xor al,al
;;     mov cx,4
;; .loop:
;;     shl di, 1
;;     rcl al, 1
;;     dec cx
;;     jnz .loop

;;     cmp al, 9
;;     jg .alpha
;;     add al, '0'
;;     jmp .hexification_done

;; .alpha:    
;;     sub al, 10
;;     add al, 'a'
;; .hexification_done:
;;     push bx
;;     ;; al is byte
;;     mov ah, 0eh
;;     mov bh, 00h
;;     mov bl, 09h
;;     int 10h

;;     pop bx
;;     dec bx
;;     jnz .print_nibble

;;     ret

print_hex8:  
    ;; args:
    ;;   dl - number
    pusha
    mov bx, 2

.print_nibble:
    xor al,al
    mov cx,4
.loop:
    shl dl, 1
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

    popa

    ret


print_char: 
    mov ah, 0eh
    mov bh, 00h
    mov bl, 09h
    int 10h
    ret

print_str:  
    pusha
.loop:
    mov al, [di]
    and al,al
    jz .done
    call print_char
    inc di
    jmp .loop
.done:
    popa
    ret

read_sectors:
    pusha                       ; save all registers
    ;; args:
    ;;   start sector ("lba")
    ;;   ax    - lba
    ;;   dl    - number of sectors
    ;;   es:bx - destination address

    ;; lba to chs computation
    ;;   sector = 1 + (lba % sectors_per_track)
    ;;   head_track_index = lba / sectors_per_track
    ;;   head = head_track_index % number_of_heads
    ;;   cylinder = head_track_index / number_of_heads

    div byte [SECTORS_PER_TRACK_ADDR] ; al=lba/track_size , ah = lba%sectors_per_track
    mov cl, ah
    inc cl                       ; cl = 1 + lba%track_size  (sector)

    mov ah,0                    ; ax is now 0x00:(lba/track_size)
    div byte [NUMBER_OF_HEADS_ADDR]  ; al = (lba/track_size)/ n_heads, ah = (lba/track_size) % n_heads
    mov dh, ah                  ; head number
    mov ch, al                  ; cylinder number (assume < 256)
    mov al, dl                  ; number of sectors to read
    mov dl, 0                   ; drive number
    mov ah, 2                   ; operation is read
    int 13h
    popa                        ; restore all registers
    ret

msg:
    db "banan",0
line_end_str:
    db `\r\n\0`

kernel_filename:
    db "kernel.bin"
times 510-CODE_START_OFFSET-($-$$) db 0
;;     dw 0AA55h
