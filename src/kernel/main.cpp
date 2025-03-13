#include <stdint.h>     //with GCC, stdint becomes a standard file
#include <std/stdio.h>
#include <i686/io.h>
#include <std/string.h>
#include<hal/hal.hpp>
#include<i686/irq.hpp>

extern uint8_t __bss_start;
extern uint8_t __end;

void end(){
    for(;;);
}

void timer_handler(){
    printf(".");
}

/**
 * Since IRQ0 is connected to a timer, we want to set a custom handler for it, so that we don't get constant interrupt messages.
 */

extern "C" void __attribute__((section(".entry"))) start(uint16_t boot_drive){
    memset(&__bss_start, 0, (&__end)-(&__bss_start));

    hal_init();
    //i686_irq_register_handler(0, timer_handler);

    cls();
    printf("Hi from kernel!\n");
    //i686_crash();


    end();
}
