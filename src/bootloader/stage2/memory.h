#pragma once

#include "stdint.h"

void far* memcpy(void far* dst, const void far* src, uint16_t num);
void far* memset(void far* ptr, int value, uint16_t size);
int memcmp(const void far* one, const void far* two, uint16_t size);
