#pragma once
#include<stdint.h>

typedef struct {
    uint16_t base_low;
    uint16_t segment_selector;
    uint8_t reserved;
    uint8_t flags;
    uint8_t base_high;
}__attribute__((packed)) idt_entry;

typedef struct {
    uint16_t limit;
    idt_entry* table;
}__attribute__((packed)) idt_descr;

typedef enum {
    IDT_FLAGS_GATE_TASK             =0x5,
    IDT_FLAGS_GATE_16BIT_INT        =0x6,
    IDT_FLAGS_GATE_16BIT_TRAP       =0x7,
    IDT_FLAGS_GATE_32BIT_INT        =0xe,
    IDT_FLAGS_GATE_32BIT_TRAP       =0xf,

    IDT_FLAGS_RING0                 =(0<<5),
    IDT_FLAGS_RING1                 =(0<<6),
    IDT_FLAGS_RING2                 =(0<<7),
    IDT_FLAGS_RING3                 =(0<<8),

    IDT_FLAG_PRESENT                =0x80
} idt_flags;

#define IDT_SIZE 256

//=======================================================

void i686_idt_init();
void i686_idt_setgate(int interrno, void* base, uint16_t seg_descr, uint8_t flags);
void i686_idt_enablegate(int interrno);
void i686_idt_disablegate(int interrno);