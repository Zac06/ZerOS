#include <i686/gdt.h>
#include<stdint.h>


gdt_entry g_gdt[]={         //practically, the descriptor table
    GDT_ENTRY(0, 0, 0, 0),  //NULL descriptor

    //kernel 32-bit code segment
    GDT_ENTRY(0, 0xfffff, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE, GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K),

    //kernel 32-bit data segment
    GDT_ENTRY(0, 0xfffff, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE, GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K)

};

gdt_descr g_gdtdescriptor={sizeof(g_gdt)-1, g_gdt};

void __attribute__((cdecl)) i686_gdt_load(gdt_descr* descr, uint16_t codeseg, uint16_t dataseg);

void i686_gdt_init(){
    i686_gdt_load(&g_gdtdescriptor, i686_GDT_CODE_SEGMENT, i686_GDT_DATA_SEGMENT);
}