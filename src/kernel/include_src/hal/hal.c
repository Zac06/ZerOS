#include <hal/hal.h>
#include<i686/gdt.h>


void hal_init(){
    i686_gdt_init();
}