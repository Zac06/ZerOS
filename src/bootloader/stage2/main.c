#include <stdint.h>     //with GCC, stdint becomes a standard file
#include "stdio.h"
#include "memory.h"
#include "x86.h"
#include "fat.h"
#include "memdefs.h"
#include "memory.h"

void* g_data=(void*)0x20000;

uint8_t* krnl_loadbuf=(uint8_t*)MEM_LOAD_KERNEL;
uint8_t* krnl=(uint8_t*)MEM_KERNEL_ADDR;

typedef void (*krnl_start)();

void end(){
    for(;;);
}

void __attribute__((_cdecl)) stage2_c(uint16_t boot_drive){
    cls();
    
    disk dsk;
    if(!disk_init(&dsk, boot_drive)){
        printf("disk init error\r\n");
        end();
    }


    if(!fat_init(&dsk)){
        printf("fat init error\r\n");
        end();
    }

    printf("\n\n");

    //load the kernel
    uint32_t read_bytes;
    uint8_t* kernelbuf=krnl;

    fat_file* fd=fat_open(&dsk, "/kernel.bin");

    while((read_bytes=fat_read(&dsk, fd, MEM_LOAD_SIZE, krnl_loadbuf))){
        memcpy(kernelbuf, krnl_loadbuf, read_bytes);
        kernelbuf+=read_bytes;
    }
    fat_close(fd);

    krnl_start kernelstart=(krnl_start)krnl;
    kernelstart();

    end();
}
