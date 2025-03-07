#pragma once

#include<stdint.h>

#define i686_GDT_CODE_SEGMENT   0x8         //index of segment multiplied by descriptor size (1*8)
#define i686_GDT_DATA_SEGMENT   0x10        //index of segment multiplied by descriptor size (2*8)

typedef struct {
    uint16_t limit_low;         //limit (bits 0-15)
    uint16_t base_low;          //base (bits 0-15)
    uint8_t base_middle;        //base (bits 16-23)
    uint8_t access;
    uint8_t flags_limit_high;   //limit (bits 16-19)
    uint8_t base_high;          //base (bits 24-31)

}__attribute__((packed)) gdt_entry;

typedef struct {
    uint16_t limit;             //size of gdt
    gdt_entry* ptr;             //addr of gdt
} __attribute__((packed)) gdt_descr;

typedef enum {
    GDT_ACCESS_CODE_READABLE            = 0x02,
    GDT_ACCESS_DATA_WRITEABLE           = 0x02,

    GDT_ACCESS_CODE_CONFORMING          = 0x04,
    GDT_ACCESS_DATA_DIRECTION_NORMAL    = 0x00,
    GDT_ACCESS_DATA_DIRECTION_DOWN      = 0x04,

    GDT_ACCESS_DATA_SEGMENT             = 0x10,
    GDT_ACCESS_CODE_SEGMENT             = 0x18,

    GDT_ACCESS_DESCRIPTOR_TSS           = 0x00,

    GDT_ACCESS_RING0                    = 0x00,
    GDT_ACCESS_RING1                    = 0x20,
    GDT_ACCESS_RING2                    = 0x40,
    GDT_ACCESS_RING3                    = 0x60,

    GDT_ACCESS_PRESENT                  = 0x80
    
} gdt_access;

typedef enum {
    GDT_FLAG_64BIT                      = 0x20,
    GDT_FLAG_32BIT                      = 0x40,
    GDT_FLAG_16BIT                      = 0x00,

    GDT_FLAG_GRANULARITY_1B             = 0x00,
    GDT_FLAG_GRANULARITY_4K             = 0x80
}gdt_flags;

#define GDT_LIMIT_LOW(limit)\
    (limit & 0xFFFF)


#define GDT_BASE_LOW(base)\
    (base & 0xFFFF)

#define GDT_BASE_MIDDLE(base)\
    ((base>>16) & 0xFF)

#define GDT_BASE_HIGH(base)\
    ((base>>24)&0xFF)


#define GDT_FLAGS_LIMIT_HIGH(limit, flags)\
    (((limit>>16) & 0xF) | (flags & 0xF0))


#define GDT_ENTRY(base, limit, access, flags) {\
    GDT_LIMIT_LOW(limit),\
    GDT_BASE_LOW(base),\
    GDT_BASE_MIDDLE(base),\
    access,\
    GDT_FLAGS_LIMIT_HIGH(limit, flags),\
    GDT_BASE_HIGH(base),\
}

//====================================================

void i686_gdt_init();