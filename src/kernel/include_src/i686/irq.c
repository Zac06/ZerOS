#include <i686/irq.h>
#include<i686/pic.h>
#include<i686/io.h>
#include<stddef.h>
#include<std/stdio.h>
#include<i686/isr.h>
#include<stdbool.h>

#define PIC_REMAP_OFFSET 0x20

irq_handler g_irqhandlers[16];

void i686_irq_handler(registers* regs){
    int irq=regs->interrno-PIC_REMAP_OFFSET;
    if(g_irqhandlers[irq]==NULL){
        printf("Unhandled IRQ: %d\n", irq);
    }else{
        g_irqhandlers[irq](regs);       //set handler
    }

    //send eoi, to tell the PIC that the interrupt has been handled
    i686_pic_send_eoi(irq);
}

/**
 * Since the first 32 interrupts are reserved for exceptions, we need to remap the PIC interrupts to start from 32.
 * This is done by sending the ICW2 to the PICs with the offset 32 for the master and 40 (or pic1+8) for the slave.
 */
void i686_irq_init(){
    i686_pic_cfg(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET+8, true);

    //set a common interrupt service routine for all interrupt lines
    for(int i=0; i<16; i++){
        i686_isr_register_handler(PIC_REMAP_OFFSET+i, i686_irq_handler);
    }

    i686_enaint();
}

void i686_irq_register_handler(int irq, irq_handler handler){
    g_irqhandlers[irq]=handler;
}
