#pragma once

#include <stdint.h>
#include <stdbool.h>

void __attribute__((cdecl)) i686_outb(uint16_t port, uint8_t val);
uint8_t __attribute__((cdecl)) i686_inb(uint16_t port);
void i686_halt();
void i686_crash();

void i686_iowait();
void i686_enaint();
void i686_disint();
