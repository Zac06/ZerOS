#include<hal/peripherals/keyboard/keystate_map.hpp>
#include<std/string.h>

bool keystate_map::keystates[KC_ENUM_SIZE]={};
bool keystate_map::numlocktoggle=false;
bool keystate_map::capslocktoggle=false;

void keystate_map::init(){
    memset(keystates, 0, sizeof(keystates));
}

void keystate_map::press(int kc)
{
    keystates[kc]=true;
    if(kc==KC_NUMLOCK){
        numlocktoggle=!numlocktoggle;
    }else if(kc==KC_CAPSLOCK){
        capslocktoggle=!capslocktoggle;
    }
}

void keystate_map::release(int kc)
{
    keystates[kc]=false;
}

bool keystate_map::ispressed(int kc)
{
    if(kc==KC_NUMLOCK){
        return numlocktoggle;
    }else if(kc==KC_CAPSLOCK){
        return capslocktoggle;
    }
    return keystates[kc];
}