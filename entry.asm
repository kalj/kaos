bits 16

section .entry    

global entry    

extern kmain

SYSTEM_INFO_SEGMENT  equ 0x0
LOWMEM_SIZE_ADDRESS  equ 0x500
MMAP_NUM_OFFSET      equ 0x502
MMAP_DATA_OFFSET     equ 0x504
MMAP_ENTRY_SIZE      equ 24

entry:  

    ;; setup stack
    mov ax, ds
    mov ss, ax
    mov sp, 0x7000
    mov bp, sp

    ;;  gather system info and write to 0x0:0x500
    mov eax, SYSTEM_INFO_SEGMENT
    mov es, eax
    mov ds, eax

    ;; get size of low memory
    int 0x12
    mov [LOWMEM_SIZE_ADDRESS], ax

    ;; get memory map
    mov di, MMAP_DATA_OFFSET

    call read_mem_map
    mov [MMAP_NUM_OFFSET], ax

    mov di, MEM_MAP_NUM_STR
    call print_str
    mov di, ax
    call print_hex16
    call print_newline


    cli         ; disable interrupts

    call setupA20
    mov di, msg_a20_success 
    call print_str

    ;; set up gdt
    lgdt [gdtr]

    ;; enable protected mode
    mov eax, cr0
    or eax, 0x1                 ; set bit 1
    mov cr0, eax

    jmp dword 0x08:protected_mode     ; selector 1, in GDT, Privilege level 0

MEM_MAP_NUM_STR:
    db `Number of mem map entries read: \0`

read_mem_map:   
    ;; args:
    ;;   es - dst segment
    ;;   di - dst offset

    ;; cdecl; eax, ecx, and edx can be trashed
    push ebx

    xor ebx, ebx                ; start value 0

.mem_map_next:
    mov ecx, MMAP_ENTRY_SIZE ; size of each_entry
    mov eax, 0xe820
    mov edx, "PAMS" ;0x534D4150  ; 'SMAP'
    int 0x15

    ;; ebx == 0, done
    jc .mem_map_error_carry

    cmp eax, "PAMS" ;0x534D4150         ; 'SMAP'
    jne .mem_map_error_smap

    cmp ecx, 20
    jl .mem_map_error_overrun
    cmp ecx, MMAP_ENTRY_SIZE
    jg .mem_map_error_underrun

    ;; success
    cmp ebx, 0
    je .mem_map_done

    add di, MMAP_ENTRY_SIZE
    jmp .mem_map_next

.mem_map_error_carry:
    mov di, .carry_error_str
    jmp .error
.mem_map_error_smap:
    mov di, .smap_error_str
    jmp .error
.mem_map_error_overrun:
    mov di, .overrun_error_str
    jmp .error
.mem_map_error_underrun:
    mov di, .underrun_error_str
    jmp .error

.error:
    call print_str
.infloop:
    hlt
    jmp .infloop
    
.mem_map_done:
    mov ax, di
    sub ax, MMAP_DATA_OFFSET
    mov bl, 24
    xor dx,dx
    div bl

    pop ebx

    ret

.carry_error_str:
    db `Error reading memory map: Carry set\r\n\0`
.smap_error_str:
    db `Error reading memory map: Missing SMAP\r\n\0`
.overrun_error_str:
    db `Error reading memory map: Overrun\r\n\0`
.underrun_error_str:
    db `Error reading memory map: Underrun\r\n\0`

gdt:
.gdt_null:
    dq 0x0                      ; 8 bytes, all 0

.gdt_code:
    dw 0xffff                   ; limit 0-15
    dw 0x0                      ; base 0-15
    db 0x0                      ; base 16-23
    db 0b10011010               ; access byte: P;DPL;S;E;DC;RW;A
    db 0b11001111               ; limit 16-19, flags: G;DB;L;x
    db 0x0                      ; base 24-31

.gdt_data:
    dw 0xffff                   ; limit 0-15
    dw 0x0                      ; base 0-15
    db 0x0                      ; base 16-23
    db 0b10010010               ; access byte: P;DPL;S;E;DC;RW;A
    db 0b11001111               ; limit 16-19, flags: G;DB;L;x
    db 0x0                      ; base 24-31
gdt_end:    

gdtr:   
    ;; size -1
    dw gdt_end-gdt-1
    ;; offset
    dd gdt

%include "utils.asm"

%include "a20setup.asm"    

msg_a20_success:  
    db `Successfully set up A20\r\n\0`

    
bits 32

protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    jmp kmain
    cli
    hlt

