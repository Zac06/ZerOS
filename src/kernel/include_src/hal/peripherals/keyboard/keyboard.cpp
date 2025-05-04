#include<hal/peripherals/keyboard/keyboard.hpp>

keyboard_layout keyboard::kl=keyboard_layout(KB_LAYOUT_en_US);


void keyboard::handle_press(uint8_t kc){
    char c;
        if(keystate_map::ispressed(KC_LSHIFT)||keystate_map::ispressed(KC_RSHIFT)){
            c=kl.getval(kc, KC_LSHIFT);
        }else if(keystate_map::ispressed(KC_CAPSLOCK)){
            c=kl.getval(kc, KC_CAPSLOCK);
        }else{
            c=kl.getval(kc);
        }

        if(c!=0){
            printf("%c", c);
        }
}

void keyboard::handle_release(uint8_t kc){
    
}

void keyboard::set_layout(keyboard_layout l){
    kl=l;
}

keyboard_layout keyboard::get_layout(){
    return kl;
}