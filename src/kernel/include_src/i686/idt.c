#include<i686/idt.h>
#include<stdint.h>
#include<util/binary.h>

idt_entry g_idt[IDT_SIZE];
idt_descr g_idtdescriptor={sizeof(g_idt)-1, g_idt};

void __attribute__((cdecl)) i686_idt_load(idt_descr* descriptor);

void i686_idt_setgate(int interrno, void* base, uint16_t seg_descr, uint8_t flags){
    g_idt[interrno].base_low=((uint32_t)base)&0xffff;
    g_idt[interrno].base_high=((uint32_t)base>>16)&0xffff;

    g_idt[interrno].segment_selector=seg_descr;
    g_idt[interrno].reserved=0;
    g_idt[interrno].flags=flags;
}

//manipulate single interrupts. If an interrupt gets called while disabled, then don't perform it, and/or return an exception.
void i686_idt_enablegate(int interrno){
    FLAG_SET(g_idt[interrno].flags, IDT_FLAG_PRESENT);
}

void i686_idt_disablegate(int interrno){
    FLAG_UNSET(g_idt[interrno].flags, IDT_FLAG_PRESENT);
}

void i686_idt_init(){
    i686_idt_load(&g_idtdescriptor);
}