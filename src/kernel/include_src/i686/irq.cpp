#include<i686/irq.hpp>
#include<i686/pic.hpp>
#include<i686/io.h>
#include<std/stdio.h>
#include<stddef.h>

irq_handler irq_driver::a_irqhandlers[IRQ_HANDLE_COUNT]={};
static irq_handler* g_irqhandlers;

void i686_irq_handler(registers* regs){
    int irq=regs->interrno-PIC_REMAP_OFFSET;
    if(g_irqhandlers[irq]==NULL){
        printf("Unhandled IRQ: %d\n", irq);
    }else{
        g_irqhandlers[irq](regs);       //set handler
    }

    //send eoi, to tell the PIC that the interrupt has been handled
    pic_driver::send_eoi(irq);
}

void irq_driver::init(){
    pic_driver::init(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET+8, false);
    g_irqhandlers=a_irqhandlers;

    //set a common interrupt service routine for all interrupt lines
    for(int i=0; i<IRQ_HANDLE_COUNT; i++){
        i686_isr_register_handler(PIC_REMAP_OFFSET+i, i686_irq_handler);
    }

    i686_enaint();

    pic_driver::unmask(0);
    pic_driver::unmask(1);
}

void irq_driver::register_handler(int irq, irq_handler handler){
    a_irqhandlers[irq]=handler;
}