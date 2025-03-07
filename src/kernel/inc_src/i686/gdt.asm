[bits 32]

global i686_gdt_load
i686_gdt_load:
    ; new call frame (for cdecl)
    push ebp
    mov ebp, esp

    ;load gdt
    mov eax, [ebp+8]
    lgdt [eax]

    ;reload code seg. To perform this, we have to make a far jump.
    mov eax, [ebp+12]
    push eax            ;tricking the stack into making a far jump: using the retf instruction, it takes the addess of the .reload_cs codepiece, hence it jumps to it
    push .reload_cs
    retf
    .reload_cs:


    mov ax, [ebp+16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ;restore old call frame (for cdecl)
    mov esp, ebp
    pop ebp
    ret