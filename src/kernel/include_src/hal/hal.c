#include <hal/hal.h>
#include<i686/gdt.h>
#include<i686/idt.h>
#include<i686/isr.h>

void hal_init(){
    i686_gdt_init();
    i686_idt_init();
    i686_isr_init();
    //i686_GDT_Initialize();
    //i686_IDT_Initialize();
    //i686_ISR_Initialize();
}