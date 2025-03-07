
global x86_outb
global x86_inb

x86_outb:
    [bits 32]
    mov dx, [esp+4]
    mov al, [esp+8]

    out dx, al              ;send output to certain port
    ret

;=======================================================

x86_inb:
    [bits 32]
    mov dx, [esp+4]
    mov eax, 0

    in al, dx              ;take input to certain port
    ret


; ================================================================
