checkA20Enabled:
    ;; returns in ax 1 if A20 is enabled
    ;;               0 if A20 is disabled

    pushf
    push ds
    push es
    push di
    push si
 
    cli
    ;; write 0 to X=0x0:x0500==0x500 and 0xFF to Y=0xFFFF:0x0510=0x100500, i.e. 1M above.
    ;; then read X to see if it reads 0xFF or 0x00
 
    xor ax, ax ; ax = 0
    mov es, ax

    not ax ; ax = 0xFFFF
    mov ds, ax
     
    mov di, 0x500
    mov si, 0x510

    ;; backup current values
    mov al, [es:di]
    push ax
    mov al, [ds:si]
    push ax

    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF

    cmp byte [es:di], 0xFF

    ;; restore previous values
    pop ax
    mov [ds:si], al
    pop ax
    mov [es:di], al

    ;; if the write leaked to the high address, it is disabled
    mov ax, 0
    je .checkEnabledDone
    mov ax, 1
.checkEnabledDone:   
    pop si
    pop di
    pop es
    pop ds
    popf

    ret

setupA20:
    ;; Check
    call checkA20Enabled
    cmp ax, 1
    je .done

    ;; Try "Fast enable" method
    in al, 0x92
    or al, 2
    out 0x92, al    
    ;; Check
    call checkA20Enabled
    cmp ax, 1
    je .done

    ;; Try bios method
    mov ax, 0x2401
    int 0x15
    ;; Check
    call checkA20Enabled
    cmp ax, 1
    je .done

    ;; Try write-to-0xee method
    in al,0xee
    ;; Check
    call checkA20Enabled
    cmp ax, 1
    je .done

    ;; Try the keyboard controller method
    call enableA20KeyboardMethod
    ;; Check
    call checkA20Enabled
    cmp ax, 1
    je .done

    ;; if we got here we have failed
    mov di,.msg_setting_up_a20_failed
    call print_str
.fail:
    cli
    hlt
    jmp .fail
    
.done:
    ret
.msg_setting_up_a20_failed:
    db `Enabling A20 using int 0x10 is not supported!\r\n\0`


kbCtlDataPort equ 0x60
kbCtlCommandPort equ 0x64
kbCtlCmdDisable equ 0xAD
kbCtlCmdEnable equ 0xAE
kbCtlCmdRead equ 0xD0
kbCtlCmdWrite equ 0xD1

kbCtlWaitInput:
    ;; wait until status bit #1 (input buffer status) is clear
    in al, kbCtlCommandPort
    test al, 2
    jnz kbCtlWaitInput
    ret

kbCtlWaitOutput:
    ;; wait until status bit #0 (output buffer status) is set
    in al, kbCtlCommandPort
    test al, 1
    jz kbCtlWaitOutput
    ret

    
enableA20KeyboardMethod:

    ;; disable keyboard
    call kbCtlWaitInput
    mov al, kbCtlCmdDisable
    out kbCtlCommandPort, al

    ;; send command to read
    call kbCtlWaitInput
    mov al, kbCtlCmdRead
    out kbCtlCommandPort, al

    ;; read data when ready
    call kbCtlWaitOutput
    in al, kbCtlDataPort
    push eax

    ;; send command to write byte
    call kbCtlWaitInput
    mov al, kbCtlCmdRead
    out kbCtlCommandPort, al

    ;; write data back with bit #1 set
    call kbCtlWaitInput
    pop eax
    or al, 2
    out kbCtlDataPort, al

    ;; reenable keyboard
    call kbCtlWaitInput
    mov al, kbCtlCmdEnable
    out kbCtlCommandPort, al

    ret
