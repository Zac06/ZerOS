#include <i686/isr.h>

typedef void (*irq_handler)(registers* regs);

void i686_irq_init();
void i686_irq_register_handler(int irq, irq_handler handler);
