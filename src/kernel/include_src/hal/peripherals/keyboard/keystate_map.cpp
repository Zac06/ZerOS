#include<hal/peripherals/keyboard/keystate_map.hpp>
#include<std/string.h>

bool keystate_map::keystates[KC_ENUM_SIZE]={};

void keystate_map::init(){
    memset(keystates, 0, sizeof(keystates));
}

void keystate_map::press(int kc)
{
    keystates[kc]=true;
}

void keystate_map::release(int kc)
{
    keystates[kc]=false;
}

bool keystate_map::ispressed(int kc)
{
    return keystates[kc];
}