#pragma once
#include<stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t base_low;
    uint16_t segment_selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t base_high;
}__attribute__((packed)) idt_entry;

typedef struct {
    uint16_t limit;
    idt_entry* ptr;
}__attribute__((packed)) idt_descr;

typedef enum {
    IDT_FLAG_GATE_TASK             =0x5,
    IDT_FLAG_GATE_16BIT_INT        =0x6,
    IDT_FLAG_GATE_16BIT_TRAP       =0x7,
    IDT_FLAG_GATE_32BIT_INT        =0xe,
    IDT_FLAG_GATE_32BIT_TRAP       =0xf,

    IDT_FLAG_RING0                 =(0<<5),
    IDT_FLAG_RING1                 =(1<<5),
    IDT_FLAG_RING2                 =(2<<5),
    IDT_FLAG_RING3                 =(3<<5),

    IDT_FLAG_PRESENT                =0x80
} idt_flags;

#define IDT_SIZE 256

//=======================================================

void i686_idt_init();
void i686_idt_setgate(int interrno, void* base, uint16_t seg_descr, uint8_t flags);
void i686_idt_enablegate(int interrno);
void i686_idt_disablegate(int interrno);

#ifdef __cplusplus
}
#endif