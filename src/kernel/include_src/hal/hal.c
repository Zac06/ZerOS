#include <hal/hal.h>
#include<i686/gdt.h>
#include<i686/idt.h>
#include<i686/isr.h>
#include<i686/irq.h>

void hal_init(){
    i686_gdt_init();
    i686_idt_init();
    i686_isr_init();
    i686_irq_init();
}