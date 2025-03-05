%macro x86_enter_realmode 0
    [bits 32]
    jmp word 0x18:.pmode16

.pmode16:
    [bits 16]

    cli

    mov eax, cr0
    and al, ~1
    mov cr0, eax

    jmp word 0x00:.rmode

.rmode:
    mov ax, 0
    mov ds, ax
    mov ss, ax
    sti

%endmacro

%macro x86_enter_protectedmode 0
    cli

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp dword 08h:.pmode


.pmode:
    [bits 32]

    mov ax, 0x10
    mov ds, ax
    mov ss, ax

%endmacro

; convert linear address to segment offset address
; args:
;   1 - [in]    linear address
;   2 - [out]   target segment
;   3 - [in]    target 32-bit register
;   4 - [in]    target lower 16-bit half of #3

%macro linear_to_segoffset 4
    
    mov %3, %1
    shr %3, 4
    mov %2, %4
    mov %3, %1
    and %3, 0xf

%endmacro

global x86_outb
global x86_inb
global x86_disk_getdriveparams
global x86_disk_reset
global x86_disk_read

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


x86_disk_getdriveparams:
    [bits 32]

    push ebp
    mov ebp, esp

    x86_enter_realmode

    [bits 16]

    ;save regs
    push es
    push bx
    push esi
    push di

    mov dl, [bp+8]      ;drive no
    mov ah, 0x08        ;int 13,8
    mov di, 0           ;prevent bios bugs
    mov es, di          ;prevent bios bugs

    stc
    int 0x13

    

    ;out parameters

    linear_to_segoffset [bp+12], es, esi, si
    ;mov si, [bp+6]      ;drive type ptr
    mov es:[si], bl        ;copy into memory the drive type

    mov bl, ch          ;low bits in ch for cylinder no
    mov bh, cl          ;high bits in cl (6-7) for cylinder no
                        ;other bits are for sectors
    shr bh, 6
    inc bx

    linear_to_segoffset [bp+16], es, esi, si
    ;mov si, [bp+8]      ;save bits in cylinder_out ptr
    mov es:[si], bx

    mov ch, 0
    and cl, 0x3f        ;low bits (sectors)
    linear_to_segoffset [bp+20], es, esi, si
    ;mov si, [bp+10]     ;save sectors in sectors_out ptr
    mov es:[si], cx

    mov cl, dh          ;heads
    inc cx

    linear_to_segoffset [bp+24], es, esi, si
    ;mov si, [bp+12]     ;save heads in heads_out ptr     
    mov es:[si], cx

    ;restore regs
    pop di
    pop esi
    pop bx
    pop es

    mov eax, 1           ;check retval of interrupt
    sbb eax, 0
    push eax

    x86_enter_protectedmode

    [bits 32]

    pop eax

    mov esp, ebp
    pop ebp
    ret

; ================================================================

x86_disk_reset:
    [bits 32]

    push ebp
    mov ebp, esp

    x86_enter_realmode

    mov ah, 0
    mov dl, [bp+8]      ;drive
    stc                 ;make sure interrupts are enabled
    int 0x13

    mov eax, 1
    sbb eax, 0           ;1 on success, 0 on fail

    push eax

    x86_enter_protectedmode

    pop eax

    mov esp, ebp
    pop ebp
    ret

; ================================================================

x86_disk_read:
    [bits 32]

    push ebp
    mov ebp, esp

    x86_enter_realmode

    ;save registers
    push ebx
    push es

    mov dl, [bp+8]      ;drive
    mov ch, [bp+12]      ;cylinder (low bits)
    mov cl, [bp+13]      ;cylinder (high bits (6-7))
    shl cl, 6           ;make room for sector

    mov al, [bp+16]      ;sector (bits 0-5)
    and al, 0x3f
    or cl, al           ;move sectors to cl

    mov dh, [bp+20]     ;head
    
    mov al, [bp+24]     ;count

    linear_to_segoffset [bp+28], es, ebx, bx
    ;mov bx, [bp+28]     ;es:bx is ptr to data out (which is 4 bytes, i think)
    ;mov es, bx          
    ;mov bx, [bp+32]

    mov ah, 0x02        ;int 13,2

    stc                 ;make sure interrupts are enabled
    int 0x13

    mov eax, 1
    sbb eax, 0           ;1 on success, 0 on fail

    ;restore registers
    pop es
    pop ebx

    push eax
    x86_enter_protectedmode
    pop eax

    mov esp, ebp
    pop ebp
    ret
