#pragma once

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

void* memcpy(void* dst, const void* src, uint16_t num);
void* memset(void* ptr, int value, uint16_t size);
int memcmp(const void* one, const void* two, uint16_t size);

#ifdef __cplusplus
}
#endif