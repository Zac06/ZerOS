#pragma once

#include <stdint.h>
#include <stdarg.h>

void putc(char c);
void puts(const char* str);

void printf(const char* fmt, ...);

void cls();
void setcrs(int x, int y);
