#pragma once

#include "stdint.h"
#include "stdbool.h"

void _cdecl x86_video_writechar_tty(const char c, uint8_t page);
void _cdecl x86_video_writestr_tty(const char* str, uint16_t len, uint8_t page, uint8_t attr);


void _cdecl x86_div64_32(uint64_t dividend, uint32_t divisor, uint64_t* quotient_out, uint32_t* remainder_out);

bool _cdecl x86_disk_reset(uint8_t drive);
bool _cdecl x86_disk_read(uint8_t drive, uint16_t cylinder, uint16_t sector, uint16_t head, uint8_t count, void far* data_out);
bool _cdecl x86_disk_getdriveparams(uint8_t drive, uint8_t* drtype_out, uint16_t* cyl_out, uint16_t* sect_out, uint16_t* heads_out);
