bits 16

section _TEXT class=CODE

global _x86_video_writechar_tty
global _x86_video_writestr_tty
global _x86_div64_32

global _x86_disk_reset
global _x86_disk_read
global _x86_disk_getdriveparams

global __U4D            ;this is to solve some problems relative to the __U4D undefined reference error from OpenWatcom
global __U4M            ;this also


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

; ================================================================

_x86_disk_read:
    push bp
    mov bp, sp

    ;save registers
    push bx
    push es

    mov dl, [bp+4]      ;drive
    mov ch, [bp+6]      ;cylinder (low bits)
    mov cl, [bp+7]      ;cylinder (high bits (6-7))
    shl cl, 6           ;make room for sector

    mov al, [bp+8]      ;sector (bits 0-5)
    and al, 0x3f
    or cl, al           ;move sectors to cl

    mov dh, [bp+10]     ;head
    
    mov al, [bp+12]     ;count

    mov bx, [bp+16]     ;es:bx is far ptr to data out (which is 4 bytes, i think)
    mov es, bx          
    mov bx, [bp+14]

    mov ah, 0x02        ;int 13,2

    stc                 ;make sure interrupts are enabled
    int 0x13

    mov ax, 1
    sbb ax, 0           ;1 on success, 0 on fail

    ;restore registers
    pop es
    pop bx

    mov sp, bp
    pop bp
    ret

; ================================================================

_x86_disk_reset:
    push bp
    mov bp, sp

    mov ah, 0
    mov dl, [bp+4]      ;drive
    stc                 ;make sure interrupts are enabled
    int 0x13

    mov ax, 1
    sbb ax, 0           ;1 on success, 0 on fail


    mov sp, bp
    pop bp
    ret

; ================================================================

_x86_disk_getdriveparams:
    push bp
    mov bp, sp

    ;save regs
    push es
    push bx
    push si
    push di

    mov dl, [bp+4]      ;drive no
    mov ah, 0x08        ;int 13,8
    mov di, 0           ;prevent bios bugs
    mov es, di          ;prevent bios bugs

    stc
    int 0x13

    mov ax, 1           ;check retval of interrupt
    sbb ax, 0

    ;out parameters
    mov si, [bp+6]      ;drive type ptr
    mov [si], bl        ;copy into memory the drive type

    mov bl, ch          ;low bits in ch for cylinder no
    mov bh, cl          ;high bits in cl (6-7) for cylinder no
                        ;other bits are for sectors
    shr bh, 6
    mov si, [bp+8]      ;save bits in cylinder_out ptr
    mov [si], bx

    mov ch, 0
    and cl, 0x3f        ;low bits (sectors)
    mov si, [bp+10]     ;save sectors in sectors_out ptr
    mov [si], cx

    mov cl, dh          ;heads
    mov si, [bp+12]     ;save heads in heads_out ptr     
    mov [si], cx

    ;restore regs
    pop di
    pop si
    pop bx
    pop es

    mov sp, bp
    pop bp
    ret

; =============================================================

__U4D:
    shl edx, 16         ;dx to upper half of edx
    mov dx, ax          ;edx = dividend
    mov eax, edx        ;eax = dividend
    mov edx, 0

    shl ecx, 16         ;cx to upper half of edx
    mov cx, bx          ;ecx = divisor

    div ecx
    mov ebx, edx
    mov ecx, edx
    shr ecx, 16

    mov edx, eax
    shr edx, 16

    ret

; =============================================================

__U4M:
    shl edx, 16         ;dx to upper half of edx
    mov dx, ax          ;dx = m1
    mov eax, edx        ;ax = m1

    shl ecx, 16         ;cx to upper half of edx
    mov cx, bx          ;ecx = m2

    mul ecx             ;result in edx:eax (only need eax)
    mov edx, eax        ;result in edx
    shr edx, 16         ;result to upper half

    ret