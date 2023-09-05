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
;;     ;; args:
;;     ;;    di - value to print

;;     pusha
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

;;     popa
;;     ret

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
