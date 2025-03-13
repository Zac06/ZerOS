#include<std/stdio.h>
#include<stdint.h>
#include<i686/pic.hpp>
#include<i686/io.h>

uint16_t pic_driver::a_mask=0xffff;
bool pic_driver::a_auto_eoi=false;

/// @brief Sets up the PICs
/// @param offsetpic1 ICW2 for the master PIC
/// @param offsetpic2 ICW2 for the slave PIC
/// @param auto_eoi Whether to use automatic EOI or not
void pic_driver::init(uint8_t offsetpic1, uint8_t offsetpic2, bool auto_eoi){
    disable();

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
    uint8_t icw4=PIC_ICW4_8086;
    if(auto_eoi){
        icw4|=PIC_ICW4_AUTOEOI;
    }

    i686_outb(PIC1_DATA_PORT, icw4);   //8086 mode
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, icw4);     //tell slave PIC its cascade identity
    i686_iowait();

    //clear data registers: setting all masks to 0 (no interrupts masked, hence all interrupts are forwarded to the CPU)
    i686_outb(PIC1_DATA_PORT, 0x0);
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, 0x0);
    i686_iowait();

    disable();
    //setmask(0x0000);
}

/// @brief Probes the PICs to see if they are working
/// @return true if working, false if not
bool pic_driver::probe() {
    disable();

    setmask(0x1337);
    return readmask()==0x1337;
}

/// @brief Masks all interrupt lines on the PICs
void pic_driver::disable(){
    setmask(0xffff);
}

/// @brief Sends EOI on a certain interrupt line
/// @param irq 
void pic_driver::send_eoi(int irq){
    //sends EOI to both PICs, if the interrupt is on the slave PIC, send EOI to the slave and the master.
    if(irq>=8){
        i686_outb(PIC2_CMD_PORT, PIC_CMD_EOI);
    }
    i686_outb(PIC1_CMD_PORT, PIC_CMD_EOI);
}

/// @brief Masks (disables) the specified interrupt line
/// @param irq The interrupt line to mask
void pic_driver::mask(int irq){
    uint8_t currentmask;
    int port;

    a_mask|=(1<<irq);
    if(irq<8){         //this is needed to identify whether the interrupt line is on the master or slave PIC
        currentmask=a_mask&0xff;
        port=PIC1_DATA_PORT;
    }else{
        irq-=8;         //this is needed to mask the correct interrupt line on the slave PIC
        currentmask=a_mask>>8;
        port=PIC2_DATA_PORT;
    }

    i686_outb(port,  currentmask | (1 << irq));
}

/// @brief Unmasks (enables) the specified interrupt line
/// @param irq The interrupt line to unmask
void pic_driver::unmask(int irq){
    uint8_t currentmask;
    int port;

    a_mask&=~(1 << irq);
    if(irq<8){         //this is needed to identify whether the interrupt line is on the master or slave PIC
        currentmask=a_mask&0xff;
        port=PIC1_DATA_PORT;
    }else{
        irq-=8;         //this is needed to unmask the correct interrupt line on the slave PIC
        currentmask=a_mask>>8;
        port=PIC2_DATA_PORT;
    }

    i686_outb(PIC2_DATA_PORT,  currentmask & ~(1 << irq));
    
}

uint16_t pic_driver::getmask(){
    return a_mask;
}

void pic_driver::setmask(uint16_t p_mask){
    a_mask=p_mask;

    i686_outb(PIC1_DATA_PORT, a_mask&0xff);
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, a_mask>>8);
    i686_iowait();
}

uint16_t pic_driver::readmask(){
    uint16_t tmp=0;

    tmp=i686_inb(PIC1_DATA_PORT) | (i686_inb(PIC2_DATA_PORT)<<8);
    a_mask=tmp;

    return tmp;
}