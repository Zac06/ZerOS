org 0x0
bits 16

%define ENDL 0x0d, 0x0a     
;line feed (LF) and carriage return (CR)

start:
    ;segments are already setup by the bootloader. for now we also use the same stack

    ;print msg
    mov si, msg_akunamatata
    call puts

.halt:
    cli
    hlt


;===================================================================================

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


msg_akunamatata: db "akunamatataragazzi", ENDL, 0
                ;