#pragma once

#include<stdint.h>

typedef struct {
    //since the stack is "reversed", take the values in the reverse order in which they are pushed
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t interrno;
    uint32_t errno;

    //things that the CPU pushes
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
    
}__attribute__((packed)) registers;

typedef void (*isr_handler)(registers* regs);

void i686_isr_init();
void __attribute__((cdecl)) i686_isr_handler(registers* regs);
void i686_isr_register_handler(int interrno, isr_handler handler);