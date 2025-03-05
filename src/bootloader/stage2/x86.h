#pragma once

#include <stdint.h>
#include <stdbool.h>

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t val);
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);

bool __attribute__((cdecl)) x86_disk_getdriveparams(uint8_t drive, uint8_t* drtype_out, uint16_t* cyl_out, uint16_t* sect_out, uint16_t* heads_out);

bool __attribute__((cdecl)) x86_disk_reset(uint8_t drive);
bool __attribute__((cdecl)) x86_disk_read(uint8_t drive, uint16_t cylinder, uint16_t sector, uint16_t head, uint8_t count, void* lower_data_out);
bool __attribute__((cdecl)) x86_disk_getdriveparams(uint8_t drive, uint8_t* drtype_out, uint16_t* cyl_out, uint16_t* sect_out, uint16_t* heads_out);
