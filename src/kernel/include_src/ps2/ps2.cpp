#include<ps2/ps2.hpp>
#include<i686/io.h>
#include<stdint.h>
#include<util/error.h>

ps2_driver::ps2_driver(bool p_port_no)
    :port_no(p_port_no)
{
    if(port_no==PS2_2ND_CONTROLLER&&!check_port2_available()){
        terminate("PS/2 port 2 not available. Terminating.");
    }

    if(!test_controller()){
        terminate("PS/2 controller self test failed. Terminating.");
    }

    if(!test_port()){
        if(port_no == PS2_1ST_CONTROLLER){
            terminate("PS/2 port test no. 1 failed. Terminating.");
        }else{
            terminate("PS/2 port test no. 2 failed. Terminating.");
        }
    }

    disable();
    flush_outbuf();
    disable_irq();
    disable_translation();

    enable();
    enable_irq();

    if(!reset()){
        terminate("PS/2 controller reset failed. Terminating.");
    }
}

/*void ps2_driver::init(bool p_port_no){

}*/

bool ps2_driver::test_controller(){
    i686_outb(PS2_CMD_PORT, PS2_TEST_CONTROLLER);
    i686_iowait();

    uint8_t response = i686_inb(PS2_DATA_PORT);
    i686_iowait();

    if(response == PS2_CONTR_TEST_PASS){
        return true;
    }else{
        return false;
    }
}

bool ps2_driver::test_port(){
    if(port_no == PS2_1ST_CONTROLLER){
        i686_outb(PS2_CMD_PORT, PS2_TEST_PORT1);
    }else{
        i686_outb(PS2_CMD_PORT, PS2_TEST_PORT2);
    }
    i686_iowait();

    uint8_t ret=i686_inb(PS2_DATA_PORT);
    i686_iowait();

    return ret==PS2_PORT_TEST_PASS;
}

void ps2_driver::disable(){
    if(port_no == PS2_1ST_CONTROLLER){
        i686_outb(PS2_CMD_PORT, PS2_DISABLE_PORT1);
    }else{
        i686_outb(PS2_CMD_PORT, PS2_DISABLE_PORT2);
    }
    i686_iowait();
}

void ps2_driver::enable(){
    if(port_no == PS2_1ST_CONTROLLER){
        i686_outb(PS2_CMD_PORT, PS2_ENABLE_PORT1);
    }else{
        i686_outb(PS2_CMD_PORT, PS2_ENABLE_PORT2);
    }
    i686_iowait();
}

void ps2_driver::flush_outbuf(){
    while(i686_inb(PS2_CMD_PORT)&PS2_STAT_OUTBUF){
        i686_inb(PS2_DATA_PORT);
    }
    i686_iowait();
}

void ps2_driver::flush_inbuf(){
    while(i686_inb(PS2_CMD_PORT)&PS2_STAT_INBUF){
        i686_inb(PS2_DATA_PORT);
    }
    i686_iowait();
}

void ps2_driver::disable_irq(){
    i686_outb(PS2_CMD_PORT, PS2_READ_BYTE0);
    i686_iowait();

    uint8_t ps2conf=i686_inb(PS2_DATA_PORT);
    i686_iowait();

    if(port_no == PS2_1ST_CONTROLLER){
        ps2conf&=~PS2_PORT1_INTERRUPT_ENABLE;
    }else{
        ps2conf&=~PS2_PORT2_INTERRUPT_ENABLE;
    }
    
    i686_outb(PS2_CMD_PORT, PS2_WRITE_BYTE0);
    flush_inbuf();
    i686_outb(PS2_DATA_PORT, ps2conf);
    i686_iowait();
}

void ps2_driver::enable_irq(){
    i686_outb(PS2_CMD_PORT, PS2_READ_BYTE0);
    i686_iowait();

    uint8_t ps2conf=i686_inb(PS2_DATA_PORT);
    i686_iowait();

    if(port_no == PS2_1ST_CONTROLLER){
        ps2conf|=PS2_PORT1_INTERRUPT_ENABLE;
    }else{
        ps2conf|=PS2_PORT2_INTERRUPT_ENABLE;
    }
    
    i686_outb(PS2_CMD_PORT, PS2_WRITE_BYTE0);
    flush_inbuf();
    i686_outb(PS2_DATA_PORT, ps2conf);
    i686_iowait();
}

void ps2_driver::disable_translation(){
    i686_outb(PS2_CMD_PORT, PS2_READ_BYTE0);
    i686_iowait();

    uint8_t ps2conf=i686_inb(PS2_DATA_PORT);
    i686_iowait();

    ps2conf&=~PS2_PORT1_TRANSLATION_ENABLE;

    i686_outb(PS2_CMD_PORT, PS2_WRITE_BYTE0);
    flush_inbuf();
    i686_outb(PS2_DATA_PORT, ps2conf);
    i686_iowait();
}

bool ps2_driver::check_port2_available(){
    i686_outb(PS2_CMD_PORT, PS2_READ_BYTE0);
    i686_iowait();

    uint8_t ret=i686_inb(PS2_DATA_PORT);
    i686_iowait();

    return ret&PS2_CCB_2ND_PORT_CLOCK;
}

bool ps2_driver::reset(){
    i686_outb(PS2_DATA_PORT, PS2_RESET);
    i686_iowait();

    i686_inb(PS2_CMD_PORT);
    i686_iowait();
    uint8_t ret1=i686_inb(PS2_DATA_PORT);

    if(ret1==PS2_RESET_FAIL){
        terminate("PS/2 controller reset failed. Terminating.");
        return -1;
    }

    uint8_t ret2=i686_inb(PS2_DATA_PORT);
    i686_iowait();
    
    if( (ret1!=PS2_RESET_SUCCESS1||ret2!=PS2_RESET_SUCCESS2)||
        (ret1!=PS2_RESET_SUCCESS2||ret2!=PS2_RESET_SUCCESS1)){
            return true;
        }
    
    terminate("PS/2 controller reset failed. Terminating.");
    return false;
}