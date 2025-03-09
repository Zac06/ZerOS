#include<i686/pic.h>
#include<i686/io.h>

#define PIC1_CMD_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_CMD_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

/**
 * On i686 platforms, the PIC is used to handle interrupts. It's a programmable interrupt controller that can be used to handle up to 16 interrupts.
 * The PIC is initialized by sending it a series of initialization control words (ICW). The ICW is sent to the PIC by writing to the command port of the PIC.
 * The PIC has two command ports, one for the master PIC and one for the slave PIC: in fact, Intel decided to use two PICs, to give a way to handle more than 8 interrupts,
 * hence have more than 8 hardware devices connected to the system.
 * 
 * The master PIC is connected to the CPU via the IRQ0-IRQ7 lines, while the slave PIC is connected to the master PIC via the IRQ2 line (third line).
 * 
 */

//INITIALIZATION CONTROL WORD 1 (ICW1)
/**
 * What is the ICW1? And what is an initialization control word in the first place?
 * An initialization control word is a word that is sent to a device to initialize it. The ICW1 is the first word that is sent to the PIC to initialize it.
 * The ICW1 is sent to the PIC by writing to the command port of the PIC. The ICW1 is an 8-bit word that has the following format:
 * 
 * 0    IC4     if set, the PIC expects ICW4 during initialization
 * 1    SNGL    if set, the PIC operates in single mode, else cascade mode and expects ICW3 to be sent
 * 2    ADI     set: 4, not set: 8; ignored on i686, so set to 0
 * 3    LTIM    if set, the PIC operates in level triggered mode, else edge triggered mode. We want edge triggered mode
 * 4    INIT    if set, the PIC is initialized, else it is not
 * 5            set to 0, ignored on i686
 * 6            set to 0, ignored on i686
 * 7            set to 0, ignored on i686
 */

//INITIALIZATION CONTROL WORD 2 (ICW2)
/**
 * What is the ICW2?
 * The ICW2 is the second word that is sent to the PIC to initialize it. The ICW2 is sent to the PIC by writing to the data port of the PIC (A0h is set to 1). The ICW2 is an 8-bit word that has the following format:
 * 
 */

 //INITIALIZATION CONTROL WORD 3 (ICW3)
/**
 * What is the ICW3?
 * The ICW3 is the third word that is sent to the PIC to initialize it. The ICW3 is sent to the PIC by writing to the data port of the PIC (A0h is set to 1). The ICW3 is an 8-bit word that has the following format:
 * Note: The ICW3 is different for the master and slave PICs.
 * 
 */

//INITIALIZATION CONTROL WORD 4 (ICW4)
/**
 * What is the ICW4?
 * The ICW4 is the fourth word that is sent to the PIC to initialize it. The ICW4 is sent to the PIC by writing to the data port of the PIC (A0h is set to 1). The ICW4 is an 8-bit word that has the following format:
 * 
 * 0    uPM     if set, the PIC operates in 8086/88 mode, else MCS-80/85 mode
 * 1    AEOI    if set, the PIC operates in automatic EOI mode, else normal EOI mode
 * 2    MS      if set, the PIC operates in non buffered (null), master mode (1), else slave mode (1)
 * 3    BUF     if set, the PIC operates in buffered mode, else non buffered mode (specific to other platforms, irrelevant to i686)
 * 4    SFNM    if set, the PIC operates in special fully nested mode (default mode of operation), else not
 * 5            set to 0, ignored on i686
 * 6            set to 0, ignored on i686
 * 7            set to 0, ignored on i686
 */


/**
 * FULLY NESTED MODE
 * When multiple interrupts are triggered at the same time, the PIC will send the interrupt with the highest priority to the CPU, based on the interrupt line number (lowest number -> highest priority).
 * The CPU can be interrupted by a higher priority interrupt while handling a lower priority interrupt.
 * However, when an interrupt from PIC2 is being served, and another higher priority interrupt from PIC1 is triggered, all the PIC1 sees is the third interrupt line being triggered since PIC2 is connected to that.
 * This is where fully nested mode comes in. In fully nested mode, the PIC will wait until the CPU acknowledges the interrupt from PIC2 before sending the interrupt from PIC1.
 * This way, the CPU can handle the interrupt from PIC2 before handling the interrupt from PIC1.
 */

/**
 * What is an OCW? And what is the OCW1?
 * An OCW is an operation control word. It is a word that is sent to a device to control it. The OCW1 is the first word that is sent to the PIC to control it.
 * The OCW1 is sent to the PIC by writing to the command port of the PIC. The OCW1 is an 8-bit word that has the following format:
 * 
 */

/** OPERATION CONTROL WORD 1 (OCW1)
 * It's used to mask/unmask interrupts, set the interrupt priority, and set the interrupt mode.
 * Note: masking an interrupt means that the interrupt will not be sent to the CPU.
 * To mask/unmask an interrupt, we have to read the interrupt mask register (IMR) of the PIC, done from the data port,
 * then modify the IMR, and write the new IMR back to the PIC on the data port.
 * 
 */

/**
 * OPERATION CONTROL WORD 2 (OCW2)
 * We will use this for the EOI (End Of Interrupt) command.
 * The EOI command is used to tell the PIC that the interrupt has been handled by the CPU.
 * A generic EOI command is sent to the PIC by writing 0x20 to the command port of the PIC, and it handles the highest priority interrupt.
 * A specific EOI command is sent to the PIC by writing 0x60 + the interrupt number to the command port of the PIC, and it handles the specified interrupt.
 */

/**
 * OPERATION CONTROL WORD 3 (OCW3)
 * We use this to read the interrupt request register (IRR) and the in-service register (ISR) of the PIC.
 * The IRR is used to read the interrupt requests that are pending on the PIC.
 * The ISR is used to read the interrupts that are currently being serviced by the CPU.
 * Difference? "Pending" means that the interrupt has been triggered but not yet sent to the CPU, while "serviced" means that the interrupt has been sent to the CPU.
 * 
 */

enum{
    PIC_ICW1_ICW4       = 0x01,
    PIC_ICW1_SINGLE     = 0x02,
    PIC_ICW1_INTERVAL4  = 0x04,
    PIC_ICW1_LEVEL      = 0x08,
    PIC_ICW1_INIT       = 0x10
} pic_icw1;

enum{
    PIC_ICW4_8086       = 0x01,
    PIC_ICW4_AUTOEOI    = 0x02,
    PIC_ICW4_BUF_MASTER = 0x04,
    PIC_ICW4_BUF_SLAVE  = 0x00,
    PIC_ICW4_BUFFERED   = 0x08,
    PIC_ICW4_SFNM       = 0x10
} pic_icw4;

enum {
    PIC_CMD_EOI         = 0x20,
    PIC_CMD_READ_IRR    = 0x0a,
    PIC_CMD_READ_ISR    = 0x0b
} pic_cmd;

void i686_pic_cfg(uint8_t offsetpic1, uint8_t offsetpic2){
    //Send ICW1 to both PICs
    i686_outb(PIC1_CMD_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    i686_iowait();
    i686_outb(PIC2_CMD_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    i686_iowait();

    //Send ICW2 to both PICs
    i686_outb(PIC1_DATA_PORT, offsetpic1);
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, offsetpic2);
    i686_iowait();

    //Send ICW3s
    i686_outb(PIC1_DATA_PORT, 0x4);     //tell master PIC that there is a slave PIC at IRQ2
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, 0x2);     //tell slave PIC its cascade identity
    i686_iowait();

    //Send ICW4
    i686_outb(PIC1_DATA_PORT, PIC_ICW4_8086);   //8086 mode
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, PIC_ICW4_8086);     //tell slave PIC its cascade identity
    i686_iowait();

    //clear data registers: setting all masks to 0 (no interrupts masked, hence all interrupts are forwarded to the CPU)
    i686_outb(PIC1_DATA_PORT, 0x0);
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, 0x0);
    i686_iowait();
}

void i686_pic_mask(uint8_t irq){        //disabling an interrupt line
    if(irq<8){         //this is needed to identify whether the interrupt line is on the master or slave PIC
        uint8_t currentmask=i686_inb(PIC1_DATA_PORT);
        i686_outb(PIC1_DATA_PORT,  currentmask | (1 << irq));
    }else{
        irq-=8;         //this is needed to mask the correct interrupt line on the slave PIC
        uint8_t currentmask=i686_inb(PIC2_DATA_PORT);
        i686_outb(PIC2_DATA_PORT,  currentmask | (1 << irq));
    }
}

void i686_pic_unmask(uint8_t irq){        //disabling an interrupt line
    if(irq<8){         //this is needed to identify whether the interrupt line is on the master or slave PIC
        uint8_t currentmask=i686_inb(PIC1_DATA_PORT);
        i686_outb(PIC1_DATA_PORT,  currentmask & ~(1 << irq));
    }else{
        irq-=8;         //this is needed to unmask the correct interrupt line on the slave PIC
        uint8_t currentmask=i686_inb(PIC2_DATA_PORT);
        i686_outb(PIC2_DATA_PORT,  currentmask & ~(1 << irq));
    }
}

void i686_pic_disable(){
    i686_outb(PIC1_DATA_PORT, 0xff);
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, 0xff);
    i686_iowait();
}

void i686_pic_send_eoi(uint8_t irq){
    //sends EOI to both PICs, if the interrupt is on the slave PIC, send EOI to the slave and the master.
    if(irq>=8){
        i686_outb(PIC2_CMD_PORT, PIC_CMD_EOI);
    }
    i686_outb(PIC1_CMD_PORT, PIC_CMD_EOI);
}

uint16_t i686_pic_read_irq(){
    i686_outb(PIC1_CMD_PORT, PIC_CMD_READ_IRR);
    i686_outb(PIC2_CMD_PORT, PIC_CMD_READ_IRR);
    return (i686_inb(PIC2_CMD_PORT)<<8) | i686_inb(PIC1_CMD_PORT);
}

uint16_t i686_pic_read_isr( ){
    i686_outb(PIC1_CMD_PORT, PIC_CMD_READ_ISR);
    i686_outb(PIC2_CMD_PORT, PIC_CMD_READ_ISR);
    return (i686_inb(PIC2_CMD_PORT)<<8) | i686_inb(PIC1_CMD_PORT);
}