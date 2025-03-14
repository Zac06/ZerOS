#pragma once

#include<stdint.h>
#include<i686/io.h>

#define PS2_DATA_PORT 0x60
#define PS2_CMD_PORT 0x64

#define PS2_1ST_CONTROLLER 0
#define PS2_2ND_CONTROLLER 1

typedef enum {
    PS2_CCB_1ST_PORT_INTERRUPT           = (1 << 0),
    PS2_CCB_2ND_PORT_INTERRUPT           = (1 << 1),
    PS2_CCB_SYSTEM_FLAG                  = (1 << 2),
    _PS2_CCB_RESERVED_1                  = (1 << 3),
    PS2_CCB_1ST_PORT_CLOCK               = (1 << 4),
    PS2_CCB_2ND_PORT_CLOCK               = (1 << 5),
    PS2_CCB_1ST_PORT_TRANSLATION         = (1 << 6),
    _PS2_CCB_RESERVED_2                  = (1 << 7)
} ps2_controller_conf_byte_flags;

typedef enum {
    PS2_PORT1_INTERRUPT_ENABLE           = (1 << 0),
    PS2_PORT2_INTERRUPT_ENABLE           = (1 << 1),
    PS2_SYSTEM_FLAG_ENABLE               = (1 << 2),
    PS2_PORT1_CLOCK_ENABLE               = (1 << 4),
    PS2_PORT2_CLOCK_ENABLE               = (1 << 5),
    PS2_PORT1_TRANSLATION_ENABLE         = (1 << 6)
} ps2_contr_conf_byte_masks;

typedef enum {
    PS2_CONTR_TEST_PASS                       = 0x55,
    PS2_CONTR_TEST_FAIL                       = 0xFC,
    PS2_RESET_SUCCESS1                        = 0xFA,
    PS2_RESET_SUCCESS2                        = 0xAA,
    PS2_RESET_FAIL                            = 0xFC
} ps2_contr_test_response;

typedef enum {
    PS2_PORT_TEST_PASS                        = 0x00,
    PS2_PORT_TEST_CLOCK_LINE_STUCK_LOW        = 0x01,
    PS2_PORT_TEST_CLOCK_LINE_STUCK_HIGH       = 0x02,
    PS2_PORT_TEST_DATA_LINE_STUCK_LOW         = 0x03,
    PS2_PORT_TEST_DATA_LINE_STUCK_HIGH        = 0x04
} ps2_port_test_response;

typedef enum {
    PS2_COP_SYSRESET                        = (1 << 0),
    PS2_COP_A20                             = (1 << 1),
    PS2_COP_PORT2_CLOCK                     = (1 << 2),
    PS2_COP_PORT2_DATA                      = (1 << 3),
    PS2_COP_PORT1_OUTBUF                    = (1 << 4),
    PS2_COP_PORT2_OUTBUF                    = (1 << 5),
    PS2_COP_PORT1_CLOCK                     = (1 << 6),
    PS2_COP_PORT1_DATA                      = (1 << 7)
} ps2_contr_out_port_masks;

typedef enum {
    PS2_READ_BYTE0                      = 0x20,
    PS2_READ_BYTE_N                     = 0x1F,
    PS2_WRITE_BYTE0                     = 0x60,
    PS2_WRITE_BYTE_N                    = 0x1F,

    PS2_DISABLE_PORT2                   = 0xA7,
    PS2_ENABLE_PORT2                    = 0xA8,
    PS2_TEST_PORT2                      = 0xA9,
    PS2_TEST_CONTROLLER                 = 0xAA,
    PS2_TEST_PORT1                      = 0xAB,
    PS2_DIAGNOSTIC_DUMP                 = 0xAC,
    PS2_DISABLE_PORT1                   = 0xAD,
    PS2_ENABLE_PORT1                    = 0xAE,
    PS2_READ_CONTROLLER_INPUT           = 0xC0,
    PS2_COPY_INPUT_0_3_TO_STATUS_4_7    = 0xC1,
    PS2_COPY_INPUT_4_7_TO_STATUS_4_7    = 0xC2,
    PS2_READ_CONTROLLER_OUTPUT          = 0xD0,
    PS2_WRITE_CONTROLLER_OUTPUT         = 0xD1,
    PS2_WRITE_PORT1_OUTPUTBUF           = 0xD2,
    PS2_WRITE_PORT2_OUTPUTBUF           = 0xD3,
    PS2_WRITE_PORT2_INPUTBUF            = 0xD4,
    PS2_RESET                           = 0xFF
} ps2_commands;

typedef enum {
    PS2_STAT_OUTBUF                     = (1<<0),
    PS2_STAT_INBUF                      = (1<<1),
    PS2_STAT_SYSTEM                     = (1<<2),
    PS2_STAT_CMD_DATA                   = (1<<3),
    _PS2_UNKNOWN_1                      = (1<<4),
    _PS2_UNKNOWN_2                      = (1<<5),
    PS2_STAT_TIMEOUT                    = (1<<6),
    PS2_STAT_PARITY                     = (1<<7)

} ps2_status_masks;

struct ps2_dev_type {
    uint16_t id;
    const char* name;
};

static ps2_dev_type devtypes[] ={
    {0x00,              "Standard PS/2 mouse"},
    {0x03,              "Mouse with scroll wheel"},
    {0x04,              "5-button mouse"},
    {0xab|(0x83<<8),    "MF2 keyboard"},
    {0xab|(0xc1<<8),    "MF2 keyboard"},
    {0xab|(0x84<<8),    "IBM ThinkPads/Spacesaver/other \"short\" keyboards"},
    {0xab|(0x85<<8),    "NCD n-97 keyboard/122-key host connected keyboard"},
    {0xab|(0x86<<8),    "122-key keyboard"},
    {0xab|(0x90<<8),    "Japanese \"G\" keyboard"},
    {0xab|(0x91<<8),    "Japanese \"P\" keyboard"},
    {0xab|(0x92<<8),    "Japanese \"A\" keyboard"},
    {0xac|(0xa1<<8),    "NCD Sun layout keyboard"},

};

class ps2_driver {
    private:
        bool port_no;

    public:
        ps2_driver(bool p_port_no);

        static bool test_controller();
        bool test_port();
        void disable();
        void enable();

        void flush_outbuf();
        void flush_inbuf();
        void disable_irq();
        void enable_irq();
        void disable_translation();
        bool check_port2_available();
        bool reset();
};