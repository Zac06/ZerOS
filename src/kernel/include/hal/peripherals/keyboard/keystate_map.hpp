#pragma once

#include<ps2/keyboard/scancode.h>

class keystate_map {
    private:
        static bool keystates[KC_ENUM_SIZE];

    public:
        static void init();
        
        static void press(int kc);
        static void release(int kc);

        static bool ispressed(int kc);
};



