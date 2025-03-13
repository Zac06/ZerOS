#pragma once

#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
    extern "C" {
#endif

enum CLR_ATTRIBUTES {
    CLR_FG_BLACK            =0x00,
    CLR_FG_BLUE,
    CLR_FG_GREEN,
    CLR_FG_CYAN,
    CLR_FG_RED,
    CLR_FG_MAGENTA,
    CLR_FG_BROWN,
    CLR_FG_LIGHTGRAY,
    CLR_FG_GRAY,
    CLR_FG_LIGHTBLUE,
    CLR_FG_LIGHTGREEN,
    CLR_FG_LIGHTCYAN,
    CLR_FG_LIGHTRED,
    CLR_FG_LIGHTMAGENTA,
    CLR_FG_YELLOW,
    CLR_FG_WHITE,

    CLR_BG_BLACK            =0x00,
    CLR_BG_BLUE             =0x10,
    CLR_BG_GREEN            =0x20,
    CLR_BG_CYAN             =0x30,
    CLR_BG_RED              =0x40,
    CLR_BG_MAGENTA          =0x50,
    CLR_BG_BROWN            =0x60,
    CLR_BG_LIGHTGRAY        =0x70,
    CLR_BG_GRAY             =0x80,
    CLR_BG_LIGHTBLUE        =0x90,
    CLR_BG_LIGHTGREEN       =0xa0,
    CLR_BG_LIGHTCYAN        =0xb0,
    CLR_BG_LIGHTRED         =0xc0,
    CLR_BG_LIGHTMAGENTA     =0xd0,
    CLR_BG_YELLOW           =0xe0,
    CLR_BG_WHITE            =0xf0,

    CLR_DEFAULT             =CLR_FG_LIGHTGRAY | CLR_BG_BLACK
};

void putc(char c);
void puts(const char* str);

void printf(const char* fmt, ...);

void cls();
void setcrs(int x, int y);
void scrlback(int lines);

#ifdef __cplusplus
}
#endif
