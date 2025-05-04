#pragma once

#include<hal/peripherals/keyboard/keyboard_layout.hpp>
#include<hal/peripherals/keyboard/layouts/all.h>
#include<hal/peripherals/keyboard/keystate_map.hpp>
#include<std/stdio.h>

class keyboard {
    private:
        static keyboard_layout kl;

    public:
        static void handle_press(uint8_t kc);

        static void handle_release(uint8_t kc);

        static void set_layout(keyboard_layout l);

        static keyboard_layout get_layout();
};