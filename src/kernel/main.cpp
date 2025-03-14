#include <stdint.h>     //with GCC, stdint becomes a standard file
#include <std/stdio.h>
#include <i686/io.h>
#include <std/string.h>
#include<hal/hal.hpp>
#include<i686/irq.hpp>
#include<ps2/ps2.hpp>

extern uint8_t __bss_start;
extern uint8_t __end;

void end(){
    for(;;);
}

void timer_handler(registers* regs){
    //printf(".");
}

void keyboard_handler(registers* regs){
    uint8_t scancode=i686_inb(0x60);
    printf("0x%x\n", scancode);
    i686_outb(0x20, 0x20);     //send EOI to PIC
}

/**
 * Since IRQ0 is connected to a timer, we want to set a custom handler for it, so that we don't get constant interrupt messages.
 */

extern "C" void __attribute__((section(".entry"))) start(uint16_t boot_drive){
    memset(&__bss_start, 0, (&__end)-(&__bss_start));

    hal_init();
    
    irq_driver::register_handler(0, timer_handler);
    irq_driver::register_handler(1, keyboard_handler);

    cls();
    printf("Hi from kernel!\n");

    ps2_driver ps2(PS2_1ST_CONTROLLER);

    end();
}
