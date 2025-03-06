#pragma once

#include <stdint.h>

void* memcpy(void* dst, const void* src, uint16_t num);
void* memset(void* ptr, int value, uint16_t size);
int memcmp(const void* one, const void* two, uint16_t size);
