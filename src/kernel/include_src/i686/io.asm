
global i686_outb
global i686_inb
global i686_halt
global i686_crash
global i686_enaint
global i686_disint

i686_outb:
    [bits 32]
    mov dx, [esp+4]
    mov al, [esp+8]

    out dx, al              ;send output to certain port
    ret

;=======================================================

i686_inb:
    [bits 32]
    mov dx, [esp+4]
    mov eax, 0

    in al, dx              ;take input to certain port
    ret


; ================================================================

i686_halt:
    [bits 32]
    cli
    hlt

; ================================================================

i686_crash:                 ;cause a divide by zero exception
    [bits 32]
    mov eax, 0
    div eax
    ret

; ================================================================

i686_enaint:
    [bits 32]
    sti
    ret

i686_disint:
    [bits 32]
    cli
    ret