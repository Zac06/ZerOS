#include<hal/peripherals/keyboard/keyboard_layout.hpp>
#include<hal/peripherals/keyboard/layouts/all.h>
#include<util/array.hpp>
#include<util/error.h>

keyboard_layout::keyboard_layout(int p_id)
    :id(p_id)
{
    if(p_id<0||p_id>=arrsize(keyboard_layout_lookup)){
        terminate("Invalid keyboard layout id");
    }
}

uint8_t keyboard_layout::getval(int p_kc, int p_mod){
    //printf("kc=%d, mod=%d", p_kc, p_mod);
    if(p_kc<0||p_kc>=KC_ENUM_SIZE){
        terminate("Invalid keyboard code");
    }

    if(p_mod<0||p_mod>=KC_LAST_MOD){
        terminate("Invalid keyboard modifier code");
    }

    /*for(int i=0; i<KC_ENUM_SIZE; i++){
        printf("%d ", (*(keyboard_layout_lookup[id]))[KC_CAPSLOCK][i]);
    }*/

    //printf("en_US: 0x%x, map: 0x%x", en_US, (*(keyboard_layout_lookup[id]))[0][0]);
    return (*(keyboard_layout_lookup[id]))[p_mod][p_kc];
}

