#pragma once

#include<ps2/ps2_driver.hpp>
#include<i686/pic.hpp>
#include<i686/isr.h>

#include<hal/peripherals/keyboard/keyboard_layout.hpp>
#include<hal/peripherals/keyboard/layouts/all.h>
#include<hal/peripherals/keyboard/keyboard.hpp>


typedef enum {
    PS2KEY_SETLED                               =0xed,
    //PS2KEY_ECHO                                 =0xee,
    PS2KEY_SCANCODE                             =0xf0,
    PS2KEY_IDENTIFY                             =0xf2,
    PS2KEY_SETTYPE_RATE_DELAY                   =0xf3,
    PS2KEY_ENABLESCANNING                       =0xf4,
    PS2KEY_DISABLESCANNING                      =0xf5,
    PS2KEY_SETDEFAULT                           =0xf6,
    PS2KEY_SETALLTYPE_AUTOREPEAT                =0xf7,
    PS2KEY_SETALLMAKE_RELEASE                   =0xf8,
    PS2KEY_SETALLMAKEONLY                       =0xf9,
    PS2KEY_SETALLTYPE_AUTOREPEAT_MAKE_RELEASE   =0xfa,
    PS2KEY_SETSCANCODE_TYPE_AUTOREPEAT          =0xfb,
    PS2KEY_SETSCANCODE_MAKE_RELEASE             =0xfc,
    PS2KEY_SETSCANCODE_MAKEONLY                 =0xfd,
    //PS2KEY_RESEND                               =0xfe,
    PS2KEY_RESET                                =0xff
} ps2_key_cmds;

typedef enum {
    PS2KEY_LED_SCROLL                           =(1<<0),
    PS2KEY_LED_NUM                              =(1<<1),
    PS2KEY_LED_CAPS                             =(1<<2)
} ps2_key_setleds_masks;

typedef enum {
    PS2KEY_SCANCODE_GET                         =0x00,
    PS2KEY_SCANCODE_SET1                        =0x01,
    PS2KEY_SCANCODE_SET2                        =0x02,
    PS2KEY_SCANCODE_SET3                        =0x03
} ps2_key_scancode_cmds;

typedef enum {
    PS2KEY_KEYERROR                             =0x00,
    PS2KEY_INBUF_OVERRUN                        =0x00,
    PS2KEY_ECHO                                 =0xee,
    PS2KEY_ACK                                  =0xfa,
    PS2KEY_SELFTEST_FAIL                        =0xfc,
    PS2KEY_SELFTEST_FAIL2                       =0xfd,
    PS2KEY_RESEND                               =0xfe,
    PS2KEY_KEYERROR2                            =0xff,
    PS2KEY_INBUF_OVERRUN2                       =0xff
} ps2_key_special;

typedef enum {
    PS2KEY_STATE_DEFAULT                        =0,
    PS2KEY_STATE_NEXTLOOKUP,
    PS2KEY_STATE_OTHER_INPUTS
} ps2_key_states;

class ps2_keyboard : public ps2_driver {
    private:
        uint16_t devtype;
        static bool firstread;

        static uint8_t state;
        static uint8_t lookuplevel;
        static uint8_t other_inputs;

        //static keyboard_layout kl;
        static keyboard* upper_layer;

    public:
        ps2_keyboard(int p_port_no);

        int8_t get_scancodeset();
        void set_scancodeset(uint8_t p_set);

        static void int_handler(registers* regs);
};

