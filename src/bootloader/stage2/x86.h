#pragma once

#include "stdint.h"

void _cdecl x86_video_writechar_tty(int8_t c, uint8_t page);
void _cdecl x86_video_writestr_tty(const int8_t* str, uint16_t len, uint8_t page, uint8_t attr);
void _cdecl x86_div64_32(uint64_t dividend, uint32_t divisor, uint64_t* quotient_out, uint32_t* remainder_out);
