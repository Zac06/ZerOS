#include<util/error.h>
#include<std/stdio.h>
#include<i686/io.h>

void terminate(const char* msg){
    printf("\n[ERROR]: %s\n", msg);
    i686_halt();
}
