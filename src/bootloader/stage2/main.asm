bits 16

section _ENTRY class=CODE

extern _cstart_
global entry

entry:
    cli     ;disable interrupts so that the setup goes correctly

    mov ax, ds
    mov ss, ax
    mov sp, 0
    mov bp, sp
    sti     ;re-enable interrupts

    ;expect boot drive in dl; _cstart_ takes it as arguments
    mov dh, 0
    push dx
    call _cstart_

    cli
    hlt
    