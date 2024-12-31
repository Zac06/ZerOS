bits 16

section _TEXT class=CODE

global _x86_video_writechar_tty
global _x86_video_writestr_tty
global _x86_div64_32

_x86_video_writechar_tty:
    push bp         ;save old call frame
    mov bp, sp      ;init new call frame

    ;save bx
    push bx

    ;[bp]       old call frame
    ;[bp+2]     return address (2 bytes for SMALL model)
    ;[bp+4]     1st argument (char)
    ;[bp+6]     2nd argument (page)
    mov ah, 0x0e
    mov al, [bp+4]
    mov bh, [bp+6]

    int 0x10    ;push char to tty
    
    ;restore old call frame
    pop bx
    mov sp, bp
    pop bp
    ret

; ============================================================

_x86_video_writestr_tty:
    ;[bp]       old call frame
    ;[bp+2]     return address (2 bytes for SMALL model)
    ;[bp+4]     1st argument (char)
    ;[bp+6]     2nd argument (len)
    ;[bp+8]     3rd argument (page)
    ;[bp+8]     4th argument (color), high nibble: background, low nibble: foreground

    push bp
    mov bp, sp

    push ax             ;push registers
    push bx
    push cx
    push dx
    push bp

    mov bh, [bp+8]      ;page. BH is valid for both following interrupts

    ;check cursor position
    mov ah, 0x03        ;read current cursor position
    int 0x10            ;actually read cursor positions
    ;as return value:
    ;   dh=row
    ;   dl=column
    ;so no need to save/transfer the values


    ;display actual string
    mov ah, 0x13        ;write string to tty
    mov al, 1           ;string is chars only, attribute in BL, cursor moved
    mov bl, [bp+10]     ;string attributes (colors)
    mov cx, [bp+6]      ;string length

    push cx             ;save string length
    mov cx, [bp+4]      ;using CX as passing register
    mov bp, cx          ;set string pos into bp
    pop cx              ;restore string length

    int 0x10            ;actually display characters

    pop bp              ;pop all modified values
    pop dx
    pop cx
    pop bx
    pop ax

    mov sp, bp
    pop bp
    ret

; ================================================================

_x86_div64_32:
    push bp         ;save old call frame
    mov bp, sp      ;init new call frame

    push bx;

    mov eax, [bp+8]     ;eax = upper 32 bits
    mov ecx, [bp+12]    ;ecx=divisor
    mov edx, 0
    div ecx

    ;store upper 32 output bits
    mov bx, [bp+16]
    mov [bx+4], eax

    ;divide lower 32 bits
    mov eax, [bp+4]     ;eax=lower 32 bits
                        ;edx=old remainder

    div ecx

    ;store results
    mov [bx], eax
    mov bx, [bp+18]
    mov [bx], edx

    pop bx

    mov sp, bp
    pop bp
    ret
