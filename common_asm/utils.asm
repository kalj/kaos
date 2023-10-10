
print_char: 
    mov ah, 0eh
    ;; mov bh, 00h
    ;; mov bl, 09h
    mov bx, 0009h
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

line_end_str:
    db `\r\n\0`

print_newline:
    push di
    mov di, line_end_str
    call print_str
    pop di
    ret
    
    

;; dump_memory_string:  
;;     ;; start address:   ds:si
;;     ;; number of bytes: cx

;; .dump_loop:
;;     mov dl, [ds:si]

;;     mov al, dl
;;     call print_char
;;     cmp al, `\n`
;;     jne .skip_cr
;;     mov al, `\r`
;;     call print_char
;; .skip_cr:

;;     inc si
;;     dec cx
;;     jnz .dump_loop
    

;; dump_memory_hex:  
;;     ;; start address:   ds:si
;;     ;; number of bytes: cx

;; .dump_loop:
;;     mov dl, [ds:si]

;;     call print_hex8
;;     mov dl, ' '
;;     call print_char

;;     inc si
;;     test si,0xf
;;     jnz .skip_print
;;     push ds
;;     mov ax, 0
;;     mov ds, ax
;;     mov di, line_end_str
;;     call print_str
;;     pop ds
;; .skip_print:    

;;     dec cx
;;     jnz .dump_loop

;; print_hex8:  
;;     ;; args:
;;     ;;   dl - number
;;     pusha
;;     mov bx, 2

;; .print_nibble:
;;     xor al,al
;;     mov cx,4
;; .loop:
;;     shl dl, 1
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
;;     popa
;;     ret

print_hex16:  
    ;; args:
    ;;    di - value to print

    pusha
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

    popa
    ret
