#pragma once

#include <i686/isr.h>

#define PIC_REMAP_OFFSET 0x20
#define IRQ_HANDLE_COUNT 16

typedef void (*irq_handler)(registers* regs);

class irq_driver {
    private:
        static irq_handler a_irqhandlers[IRQ_HANDLE_COUNT];

    public:
        static void init();
        static void register_handler(int irq, irq_handler handler);
};