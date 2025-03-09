#include<i686/isr.h>
#include<i686/idt.h>
#include<i686/gdt.h>
#include<std/stdio.h>
#include<stddef.h>

void i686_isr_init_gates();

void i686_isr_init(){
    i686_isr_init_gates();
    for (int i = 0; i < 256; i++)
        i686_idt_enablegate(i);

    i686_idt_disablegate(0x80);
}


void __attribute__((cdecl)) i686_isr_handler(registers* regs){
    printf("Interrno: %d\n", regs->interrno);
}