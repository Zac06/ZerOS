#include <hal/hal.hpp>
#include<i686/gdt.h>
#include<i686/idt.h>
#include<i686/isr.h>
#include<i686/irq.hpp>
#include<hal/peripherals/keyboard/keystate_map.hpp>

void hal_init(){
    i686_gdt_init();
    i686_idt_init();
    i686_isr_init();
    irq_driver::init();

    keystate_map::init();
}