#include <stdint.h>     //with GCC, stdint becomes a standard file
#include <std/stdio.h>
#include <i686/io.h>
#include <std/string.h>
#include<hal/hal.h>

extern uint8_t __bss_start;
extern uint8_t __end;

void end(){
    for(;;);
}

void __attribute__((section(".entry"))) start(uint16_t boot_drive){
    memset(&__bss_start, 0, (&__end)-(&__bss_start));

    hal_init();

    cls();
    printf("Hi from kernel!\n");

    __asm("int $0x13");



    end();
}
