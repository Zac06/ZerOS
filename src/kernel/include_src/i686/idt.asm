[bits 32]

global i686_idt_load
i686_idt_load:
    ; new call frame (for cdecl)
    push ebp
    mov ebp, esp

    ;load idt
    mov eax, [ebp+8]
    lidt [eax]          ;loads the interrupt descriptor table

    ;restore old call frame (for cdecl)
    mov esp, ebp
    pop ebp
    ret