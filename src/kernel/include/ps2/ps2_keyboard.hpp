#pragma once

#include<ps2/ps2_driver.hpp>



typedef enum {
    PS2KEY_SETLED                               =0xed,
    //PS2KEY_ECHO                                 =0xee,
    PS2KEY_SCANCODE                             =0xf0,
    PS2KEY_SCANCODE_GET                         =0x00,
    PS2KEY_SCANCODE_SET1                        =0x01,
    PS2KEY_SCANCODE_SET2                        =0x02,
    PS2KEY_SCANCODE_SET3                        =0x03,
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
    PS2KEY_KEYERROR                             =0x00,
    PS2KEY_INBUF_OVERRUN                        =0x00,
    PS2KEY_ECHO                                 =0xee,
    PS2KEY_ACK                                  =0xfa,
    PS2KEY_SELFTEST_FAIL                        =0xfc|(0xfd<<8),
    PS2KEY_RESEND                               =0xfe,
    PS2KEY_KEYERROR2                            =0xff,
    PS2KEY_INBUF_OVERRUN2                       =0xff
} ps2_key_special;

class ps2_keyboard : public ps2_driver {
    private:
        uint16_t devtype;

    public:
        ps2_keyboard(int p_port_no);

};