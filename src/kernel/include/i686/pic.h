#pragma once

#include <stdint.h>

void i686_pic_cfg(uint8_t offsetpic1, uint8_t offsetpic2);
void i686_pic_mask(uint8_t irq);
void i686_pic_unmask(uint8_t irq);
void i686_pic_disable();
void i686_pic_send_eoi(uint8_t irq);
uint16_t i686_pic_read_irq();
uint16_t i686_pic_read_isr();
