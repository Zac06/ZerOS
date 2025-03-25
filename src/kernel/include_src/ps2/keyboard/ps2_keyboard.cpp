#include<ps2/keyboard/ps2_keyboard.hpp>
#include<ps2/keyboard/scancode.h>
#include<i686/io.h>
#include<stddef.h>
#include<i686/pic.hpp>
#include<i686/irq.hpp>
#include<std/stdio.h>
#include<util/error.h>
#include<util/array.hpp>

bool ps2_keyboard::firstread=false;

ps2_keyboard::ps2_keyboard(int p_port_no)
    :ps2_driver(p_port_no)
{
    pic_driver::mask(1);
    //assumes that translation is disabled.
    flush_inbuf();
    flush_outbuf();

    const int scancodeset=PS2KEY_SCANCODE_SET1;
    set_scancodeset(scancodeset);
    if(scancodeset!=get_scancodeset()){
        terminate("Could not set the scancode properly.");
    }

    irq_driver::register_handler(1, handler);
    pic_driver::unmask(1);
}

int8_t ps2_keyboard::get_scancodeset(){
    const int attempts=3;
    int i=0;

    if(!port_no){
        for(int i=0; i<attempts; i++){
            i686_outb(PS2_DATA_PORT, PS2KEY_SCANCODE);
            i686_outb(PS2_DATA_PORT, PS2KEY_SCANCODE_GET);
            i686_iowait();
            uint8_t answ=i686_inb(PS2_DATA_PORT);
            if(answ==PS2KEY_ACK){
                break;
            }else{
                printf("Getting scancode set...\n");
            }
        }
        if(i==attempts){
            terminate("Could not get the current scancode set. Terminating.");
        }

        uint8_t scancodeset=i686_inb(PS2_DATA_PORT);
        i686_iowait();

        return scancodeset;
        
    }else{
        terminate("Keyboard on port no. 2 not yet implemented. Terminating.");
    }
    
    return -1;
}

void ps2_keyboard::set_scancodeset(uint8_t p_set){
    const int attempts=3;
    int i=0;

    if(!port_no){
        for(int i=0; i<attempts; i++){
            i686_outb(PS2_DATA_PORT, PS2KEY_SCANCODE);
            i686_outb(PS2_DATA_PORT, p_set);
            i686_iowait();
            uint8_t answ=i686_inb(PS2_DATA_PORT);
            if(answ==PS2KEY_ACK){
                break;
            }else{
                printf("Setting scancode set...\n");
            }
        }
        if(i==attempts){
            terminate("Could not set the scancode set. Terminating.");
        }
        
    }else{
        terminate("Keyboard on port no. 2 not yet implemented. Terminating.");
    }
}

/**
 * Unfortunately, we need to assume that this uses scancode 2, since there's no time to implement other things.
 * Luckily, scancode set 2 is guaranteed to be supported.
 */
void ps2_keyboard::handler(registers* regs){
    if(!firstread){
        firstread=true;
        i686_inb(PS2_DATA_PORT);
        i686_iowait();
        return;
    }

    const int size=arrsize(scancode1_lookup);
    int otherinps;

    for(int i=0; i<size; i++){
        uint8_t scancode=i686_inb(PS2_DATA_PORT);
        //printf("Internal handler: 0x%x\n", scancode);

        switch(scancode1_lookup[i][scancode].operation&KC_OP_MASK){
            case KC_OP_RELEASED:
                //printf("%s", scancode1_kc_strings[scancode1_lookup[i][scancode].keycode]);
                return;
                break;

            case KC_OP_PRESSED:
                printf("%s", scancode1_kc_strings[scancode1_lookup[i][scancode].keycode]);
                return;
                break;

            case KC_OP_NEXTLOOKUP:
                break;

            case KC_OP_INVALIDSC:
                printf("INVALIDSC\n");
                return;
                break;
        }

        otherinps=(scancode1_lookup[i][scancode].operation>>4);
        //printf("Other inputs: %d", otherinps);
        for(int j=0; j<otherinps; j++){
            i686_inb(PS2_DATA_PORT);
            //printf("Extra input %d", j);
        }
    }
}