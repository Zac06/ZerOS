#pragma once

#include<ps2/keyboard/scancode.h>
#include<hal/peripherals/keyboard/keycode.h>
#include<hal/peripherals/keyboard/layouts/common.h>
#include<hal/peripherals/keyboard/layouts/en_US.h>

class keyboard_layout {
    private:
        int id;

    public:
        keyboard_layout(int p_id);

        uint8_t getval(int p_kc, int p_mod=KC_INVALID);
};