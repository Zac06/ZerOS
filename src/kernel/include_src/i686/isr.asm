;the ISR is responsible of containing the actual interrupt handlers.
;they are stubs that jump to a simple point (the interrupt address), set something up, then call a C function

[bits 32]

extern i686_isr_handler

;for instance, int 8 pushes an error code. to not mess up the stack, simplest solution would be
    ;to push a dummy error code to the stack for interrupts which do not have an error code. to 
    ;avoid rewriting this 256 times, just use macros.
%macro isr_noerror_interr 1

global i686_isr%1
i686_isr%1:
    push 0          ;dummy error code
    push %1         ;number of 0th interrupt
    jmp isr_common

%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro isr_error_interr 1

global i686_isr%1
i686_isr%1:
    ;cpu pushes autonomously the errno to the stack. no need for a dummy one
    push %1         ;number of 0th interrupt
    jmp isr_common

%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%include "include/i686/isrs_generated.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

isr_common:
    pusha           ;push edi, esi, ebp, esp, ebx, edx, ecx, eax

    mov eax, 0
    mov ax, ds
    push eax

    mov ax, 0x10    ;use data segment of the kernel
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp        ;pass esp to C function, so that all pushed values can be accessed
    call i686_isr_handler
    add esp, 4

    pop eax

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8      ;return interrno,+errorcode
    iret            ;pops cs, eip, eflags, ss, esp