org 0x7c00+CODE_START_OFFSET
bits 16

    ;; Skipping this code here:

    ;; jmp main
    ;; nop


    ;; db 'MY OEM  '               ; OEM name
BYTES_PER_SECTOR_ADDR equ (0x7c00+0x0b)
    ;; dw 512                      ; bytes per sector
SECTORS_PER_CLUSTER_ADDR equ (0x7c00+0x0d)
    ;; db 1                        ; sectors per cluster
RESERVED_SECTORS_ADDR equ (0x7c00+0xe)
    ;; dw 1                        ; reserved sectors
NUM_FATS_ADDR equ (0x7c00+0x10)
    ;; db 2                        ; num FATs
NUM_ROOTDIR_ENTRIES_ADDR equ (0x7c00+0x11)
    ;; dw 224                      ; num rootdir entries (224 for floppies, 512 for hard drives)
    ;; dw 2880                     ; tot logical sectors
    ;; db 0xf0                     ; media descriptor (f0h for floppies, f8h for hard drives)
SECTORS_PER_FAT_ADDR equ (0x7c00+0x16)
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

DATA_REGION_START_SECTOR_ADDR equ 0x7e00 ; a word
BYTES_PER_CLUSTER equ 0x7e02             ; a word
BUFFER_ADDR equ 0x7f00

KERNEL_LOAD_SEGMENT equ 0x0
KERNEL_LOAD_OFFSET equ 0x500
KERNEL_MAX_SIZE equ (0x7c00 - 0x500)

ERROR_KERNEL_FILE_NOT_FOUND equ 0x1001
ERROR_KERNEL_TOO_LARGE equ 0x1002

main:
	;;---------------------------------------------------------------------------------------------
    ;; Read root directory into buffer memory area
	;;---------------------------------------------------------------------------------------------
    ;; start sector: number of reserved sectors + num_fats*sectors_per_fat
    mov al, [NUM_FATS_ADDR]
    mov ah, 0
    mul word [SECTORS_PER_FAT_ADDR] ; discard dx 
    add ax, [RESERVED_SECTORS_ADDR]
    push ax
    
    ;; number of sectors: 1 + (num_rootdir_entries * 32 - 1) / bytes per sector
    mov ax, [NUM_ROOTDIR_ENTRIES_ADDR]
    shl ax, 5                   ; multiply by 32
    dec ax
    div word [BYTES_PER_SECTOR_ADDR] ; ax = ax/bytes_per_sector , dx = ax % bytes_per_sector
    ;; Assume <256
    inc al
    mov dl, al                  ; number of sectors to read

    pop ax                      ; restore  start sector

    mov bx, 0                   
    mov es, bx
    mov bx, BUFFER_ADDR
    call read_sectors

    ;; save sector after root dir
    mov dh, 0
    add ax, dx
    mov word [DATA_REGION_START_SECTOR_ADDR], ax

	;;---------------------------------------------------------------------------------------------
    ;; look for kernel name in root directory entries
	;;---------------------------------------------------------------------------------------------
    
    mov bx, [NUM_ROOTDIR_ENTRIES_ADDR]
    mov ax, BUFFER_ADDR
.check_entry:
    mov di, ax
    cmp byte [di], 0              ; if first byte is 0
    jz kernel_file_not_found

    mov cx, 11                  ; compare up to 11 chars
    mov di, ax
    mov si, kernel_filename
    repe cmpsb                   ; compares di, si until either is 0 or they are unequal, then zf is set if they are all equal
    je kernel_file_found

    add ax, 32
    dec bx
    jnz .check_entry

kernel_file_not_found:  
    mov ax, ERROR_KERNEL_FILE_NOT_FOUND
    jmp error_handler

kernel_file_found:  

    mov bx, ax
    ;; ensure kernel fits in available memory
    ;; mov dword edx, [bx+28]
    ;; cmp edx, KERNEL_MAX_SIZE
    ;; jle .kernel_size_ok
    cmp dword [bx+28], KERNEL_MAX_SIZE
    jle kernel_size_ok

    ;; kernel was too large to fit
    mov ax, ERROR_KERNEL_TOO_LARGE
    jmp error_handler

kernel_size_ok:
    mov di, size_success_msg
    call print_str

    ;; push edx
    ;; push dword [bx+28]
    ;; save start cluster from root dir
    mov word dx, [bx + 26]
    push dx

    mov di, read_success_msg
    call print_str


	;;---------------------------------------------------------------------------------------------
    ;; load FAT
	;;---------------------------------------------------------------------------------------------

    ;; start sector: number of reserved sectors
    mov al, [RESERVED_SECTORS_ADDR]
    mov ah, 0
    ;; number of sectors: sectors per fat
    mov dx, [SECTORS_PER_FAT_ADDR]
    mov bx, 0                   
    mov es, bx
    mov bx, BUFFER_ADDR
    call read_sectors
    
	;;---------------------------------------------------------------------------------------------
    ;; read kernel file sectors
	;;---------------------------------------------------------------------------------------------

    ;; compute bytes per cluster
    ;; Use:
    ;;   bytes per sector    - dw
    mov ah, 0
    mov al, [SECTORS_PER_CLUSTER_ADDR]
    mul word [BYTES_PER_SECTOR_ADDR]
    ;; assume dx is 0, i.e. less than 0x10000 bytes (64KiB) per cluster
    
    mov word [BYTES_PER_CLUSTER], ax

    ;; In loop:
    ;; cx is next cluster
    ;; bx is destination offset

    pop cx                      ; start cluster
    mov bx, KERNEL_LOAD_OFFSET
    
.read_cluster:
    ;; lba (sector) = data_region_start_sector + (cluster-2)*sectors_per_cluster

    ;; number of sectors to read: sectors per cluster
    mov dl, [SECTORS_PER_CLUSTER_ADDR]
    mov dh, 0

    ;; ax is sector addr
    mov ax, cx
    sub ax, 2
    mul dx
    add ax, [DATA_REGION_START_SECTOR_ADDR]

    ;; number of sectors to read: sectors per cluster
    mov dl, [SECTORS_PER_CLUSTER_ADDR]

    ;;  set write segment already
    push es
    push word KERNEL_LOAD_SEGMENT                   
    pop es
    call read_sectors
    pop es

    ;; ax, dx free to use
    mov ax, cx
    mov dx, 3
    mul word dx
    shr ax, 1                      ; ax is now (3*cluster)/2
    add ax, BUFFER_ADDR
    mov si, ax
    mov ax, [si]
    ;; if odd shift down 4 bits, else and with 0xfff
    test cx, 1                  ; check for even/odd
    jz .even_cluster
.odd_cluster:
    shr ax, 4
    jmp .fat_entry_read
.even_cluster:
    and ax, 0xfff
.fat_entry_read:

    ;; new cluster
    mov cx, ax
    ;; new destination
    add bx, [BYTES_PER_CLUSTER]

    ;; if fat entry has NOT ff8 set, then read it into cx and jmp .read_cluster
    and ax, 0xff8
    cmp ax, 0xff8
    jne .read_cluster

	;;---------------------------------------------------------------------------------------------
    ;; Jump into kernel
	;;---------------------------------------------------------------------------------------------

    ;; mov di, launch_msg
    ;; call print_str

    mov ax, KERNEL_LOAD_SEGMENT
    mov ds, ax
    mov es, ax
    ;; push ax
    ;; mov ax, KERNEL_LOAD_OFFSET
    ;; push ax
    ;; retf
    jmp KERNEL_LOAD_SEGMENT:KERNEL_LOAD_OFFSET

    cli
    hlt
.halt:
    jmp .halt

%include "utils.asm"

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

error_handler:  
    mov di, error_str
    call print_str
    mov di, ax
    call print_hex16
    call print_newline

    mov di, press_any_key_to_reboot_msg
    call print_str

    ;; read keystroke    
    mov ah, 0
    int 0x16

    ;; reboot
    int 0x19

press_any_key_to_reboot_msg:
    db `Press any key to reboot\r\n\0`

error_str:  
    db `Error: \0`
size_success_msg:    
    db `Size OK\r\n\0`
read_success_msg:    
    db `Read OK\r\n\0`

kernel_filename:
    db "KERNEL  BIN",0
;; times 510-CODE_START_OFFSET-($-$$)  
;;     dw 0AA55h
