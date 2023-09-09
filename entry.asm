bits 16
;; org 0x0

section .entry    

global entry    

extern kmain

entry:  

    ;; setup stack
    mov ax, ds
    mov ss, ax
    mov sp, 0x7000
    mov bp, sp

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

    ;; jmp dword 0x08:(0x10000+protected_mode)     ; selector 1, in GDT, Privilege level 0
    jmp dword 0x08:protected_mode     ; selector 1, in GDT, Privilege level 0

msg_the_end:     
    db `The End <3\r\n\0`

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
    

protected_mode:

bits 32
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    jmp kmain
    cli
    hlt


VGA_MEMORY equ 0xB8000
