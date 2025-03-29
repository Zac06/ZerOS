#include<ps2/keyboard/ps2_keyboard.hpp>
#include<ps2/keyboard/scancode.h>
#include<hal/peripherals/keyboard/keystate_map.hpp>
#include<i686/io.h>
#include<stddef.h>
#include<i686/pic.hpp>
#include<i686/irq.hpp>
#include<std/stdio.h>
#include<util/error.h>
#include<util/array.hpp>

#include<std/string.h>

bool ps2_keyboard::firstread=false;
uint8_t ps2_keyboard::state=PS2KEY_STATE_DEFAULT;
uint8_t ps2_keyboard::lookuplevel=0;
uint8_t ps2_keyboard::other_inputs=0;
keyboard_layout ps2_keyboard::kl=keyboard_layout(KB_LAYOUT_en_US);

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

    irq_driver::register_handler(1, int_handler);
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
 * Unfortunately, we need to assume that this uses scancode 1, since there's no time to implement other things.
 * Scancode 1 is not guaranteed to be supported.
 */
void ps2_keyboard::int_handler(registers* regs){
    if(!firstread){
        firstread=true;
        i686_inb(PS2_DATA_PORT);
        i686_iowait();
        return;
    }
    
    uint8_t scancode=i686_inb(PS2_DATA_PORT);
    i686_iowait();
    //printf(" %u - %s - ", scancode, scancode1_kc_strings[scancode1_lookup[lookuplevel][scancode].keycode]);

    switch(state){
        case PS2KEY_STATE_DEFAULT:
        case PS2KEY_STATE_NEXTLOOKUP:
            
            switch(scancode1_lookup[lookuplevel][scancode].operation&SC_OP_MASK){
                case SC_OP_RELEASED:
                    lookuplevel=0;
                    state=PS2KEY_STATE_DEFAULT;
                    keystate_map::release(scancode1_lookup[lookuplevel][scancode].keycode);
                    break;

                case SC_OP_PRESSED:
                    //printf("%s", scancode1_kc_strings[scancode1_lookup[lookuplevel][scancode].keycode]);
                    lookuplevel=0;
                    state=PS2KEY_STATE_DEFAULT;
                    keystate_map::press(scancode1_lookup[lookuplevel][scancode].keycode);

                    char c;
                    if(keystate_map::ispressed(KC_LSHIFT)||keystate_map::ispressed(KC_RSHIFT)){
                        c=kl.getval(scancode1_lookup[lookuplevel][scancode].keycode, KC_LSHIFT);
                    }else if(keystate_map::ispressed(KC_CAPSLOCK)){
                        c=kl.getval(scancode1_lookup[lookuplevel][scancode].keycode, KC_CAPSLOCK);
                    }else{
                        c=kl.getval(scancode1_lookup[lookuplevel][scancode].keycode);
                    }

                    if(c!=0){
                        printf("%c", c);
                    }

                    break;

                case SC_OP_NEXTLOOKUP:
                    lookuplevel++;
                    state=PS2KEY_STATE_NEXTLOOKUP;
                    if(lookuplevel>=arrsize(scancode1_lookup)){
                        terminate("Duh?");
                    }
                    break;

                case SC_OP_INVALIDSC:
                    //printf("INVALIDSC\n");
                    lookuplevel=0;
                    state=PS2KEY_STATE_DEFAULT;
                    
                    break;

                default:
                    terminate("Invalid operation code. Terminating.");
                    break;
        
            }

            other_inputs=(scancode1_lookup[lookuplevel][scancode].operation>>4);    //this value is supposedly different from 0 when the ps2key_op is either press or release (depends solely on the correctness of the lookup tables).
            if(other_inputs!=0){
                state=PS2KEY_STATE_OTHER_INPUTS;
            }

            break;

        case PS2KEY_STATE_OTHER_INPUTS:{
            i686_inb(PS2_DATA_PORT);
            other_inputs--;
            if(other_inputs==0){
                state=PS2KEY_STATE_DEFAULT;
            }
            break;
        }
    
    }

}