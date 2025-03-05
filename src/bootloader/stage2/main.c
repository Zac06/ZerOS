#include <stdint.h>     //with GCC, stdint becomes a standard file
#include "stdio.h"
#include "x86.h"
#include "disk.h"
#include "fat.h"

void* g_data=(void*)0x20000;

void end(){
    for(;;);
}

void __attribute__((cdecl)) stage2_c(uint16_t boot_drive){
    cls();
    puts("========= Hello world ==========\r\n");

    printf("Formatted %% %c %s\r\n", 'a', "string");
    printf("Formatted %d %i %x %p %o %hd %hi %hhu %hhd\r\n", 1234, -5678, 0xdead, 0xbeef, 012345, (short)27, (short)-42, (unsigned char)20, (signed char)-10);
    printf("Formatted %ld %lx %lld %llx\r\n", -100000000l, 0xdeadbeeful, 10200300400ll, 0xdeadbeeffeebdaedull);

    printf("\n\n");

    disk dsk;
    if(!disk_init(&dsk, boot_drive)){
        printf("disk init error\r\n");
        end();
    }

    // disk_read_sectors(&dsk, 0, 1, g_data);

    // for(int i=0; i<512; i++){
    //     printf("%x", ((unsigned char*)g_data)[i]);
    // }

    //printf("\n\n");


    if(!fat_init(&dsk)){
        printf("fat init error\r\n");
        end();
    }

    fat_file* fd=fat_open(&dsk, "/");
    fat_direntry entry;

    int j=0;
    while(fat_read_entry(&dsk, fd, &entry) && j<5){
        printf("  ");
        for(int i=0; i<11; i++){
            putc(entry.name[i]);
        }
        printf("\r\n");

        j++;
    }
    fat_close(fd);

    printf("\n\n");

    //testing subdirectory
    fd=fat_open(&dsk, "/mydir");
    j=0;
    while(fat_read_entry(&dsk, fd, &entry) && j<5){
        printf("  ");
        for(int i=0; i<11; i++){
            putc(entry.name[i]);
        }
        printf("\r\n");

        j++;
    }
    fat_close(fd);
    
    char buf[100];
    uint32_t read_bytes;
    fd=fat_open(&dsk, "/mydir/test.txt");

    while((read_bytes=fat_read(&dsk, fd, sizeof(buf), buf))){

        //printf("read_bytes: %d\r\n", read_bytes);
        for(uint32_t i=0; i<read_bytes; i++){
            if(buf[i]=='\n'){
                putc('\r');
            }
            putc(buf[i]);
        }
    }
    fat_close(fd);

    
    /*for(int i=0; i<30; i++){
        printf("hello from stage2 bootloader with GCC! %d\n", i);
    }
    uint8_t drive_type;
    uint16_t cylinders, sectors, heads;

    x86_disk_getdriveparams(boot_drive, &drive_type, &cylinders, &sectors, &heads);

    printf("Read params: DRIVETYPE=%u, CYLINDERS=%u, SECTORS=%u, HEADS=%u\n", drive_type, cylinders, sectors, heads);

    printf("hello from stage2 bootloader with GCC! %d\n", 1);
    printf("hello from stage2 bootloader with GCC! %d\n", 2);
*/
    end();
}
