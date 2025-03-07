#include <hal/hal.h>
#include<i686/gdt.h>
#include<i686/idt.h>

void hal_init(){
    i686_gdt_init();
    i686_idt_init();
}