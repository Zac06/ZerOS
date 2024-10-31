org 0x7C00
bits 16

%define ENDL 0x0d, 0x0a     
;line feed (LF) and carriage return (CR)

start:
    jmp main                ;make it so that the program always starts from main:

;===============================================================================

;prints a string to the screen.
;parameters: ds:si to string.
puts:
    push si
    push ax

.puts_loop:
    lodsb                   ;load char in AL (8 bits, hence ASCII)
    or al, al               ;check if null: if 0flag is set, then it's null
    jz .puts_done

    mov ah, 0x0e
    mov bh, 0
    int 0x10

    jmp .puts_loop

.puts_done:
    pop ax
    pop si
    ret                     ;return from where it was called

;================================================================================

main:
    ;setup data segments

    mov ax, 0
    mov dx, 0
    mov es, ax              ;cannot do mov es/ds, 0 directly

    mov ss, ax
    mov sp, 0x7c00          ;stack grows down from the start of the program

    ;print msg
    mov si, msg_akunamatata
    call puts

    hlt

.halt:
    jmp .halt


;===================================================================================


msg_akunamatata: db "akunamatataragazzi", ENDL, 0

times 510-($-$$) db 0       ;DIRECTIVE: fill the program with 0s for the first sector (512 bytes)
dw 0AA55h                   ;