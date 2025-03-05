#include <stdint.h>     //with GCC, stdint becomes a standard file
#include "stdio.h"
#include "x86.h"
#include "memory.h"

extern uint8_t __bss_start;
extern uint8_t __end;

void end(){
    for(;;);
}

void __attribute__((section(".entry"))) start(uint16_t boot_drive){
    memset(&__bss_start, 0, (&__end)-(&__bss_start));
    
    cls();
    printf("Hi from kernel!\n");

    end();
}
