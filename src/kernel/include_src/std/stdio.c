//#include "x86.h"      //in protected mode, we can't use interrupts.
                        //hence we need to write at memory addresses 
                        //for the display
#include <std/stdio.h>
#include <i686/io.h>

#include <stdint.h>
#include <stdbool.h>

uint8_t* g_screenbuf=(uint8_t*)0xb8000;     //this is the screenbuffer address
int g_screenx=0;
int g_screeny=0;
const unsigned SCR_W=80;            //in characters
const unsigned SCR_H=25;

void putclr(int x, int y, uint8_t color){
    g_screenbuf[2*(y*SCR_W+x)+1]=color;     //buffer contains 2 bytes per letter, and the second byte is the attribute one.
}

void putchr(int x, int y, char c){
    g_screenbuf[2*(y*SCR_W+x)]=c;
}

void putc(char c){
    //x86_video_writechar_tty(c, 0);
    
    switch(c){
        case '\n':
            g_screenx=0;
            g_screeny++;
            break;

        case '\r':
            g_screenx=0;
            break;

        case '\t':
            for(int i=0; i<4-(g_screenx%4); i++){
                putc(' ');
            }
            break;

        default:
            putchr(g_screenx, g_screeny, c);
            g_screenx++;
            break;
    }

    if(g_screenx>=SCR_W){
        g_screenx=0;
        g_screeny++;
    }

    if(g_screeny>=SCR_H){
        scrlback(1);
    }

    setcrs(g_screenx, g_screeny);
}

//=======================================

void puts(const char* str){
    while(*str){
        putc(*str);
        str++;
    }
}

// =====================================

void cls(){
    for(int i=0; i<SCR_H; i++){
        for(int j=0; j<SCR_W; j++){
            putchr(j,i,' ');
        }
    }
    g_screenx=0;
    g_screeny=0;
    setcrs(0,0);
}

// =====================================

void setcrs(int x, int y){
    int pos=y*SCR_W+x;

    i686_outb(0x3d4, 0xf);                       //tells something to the vga driver
    i686_outb(0x3d5, (uint8_t)(pos & 0xff));     //sets lower byte to this
    i686_outb(0x3d4, 0xe);
    i686_outb(0x3d5, (uint8_t)((pos >> 8) & 0xff));  //sdets upper byte
}

// =====================================

char getchr(int x, int y){
    return g_screenbuf[2*(y*SCR_W+x)];
}

//======================================

uint8_t getclr(int x, int y){
    return g_screenbuf[2*(y*SCR_W+x)+1];
}

// =====================================

void scrlback(int lines){
    for(int i=lines; i<SCR_H; i++){
        for(int j=0; j<SCR_W; j++){
            putchr(j, i-lines, getchr(j,i));
            putclr(j, i-lines, getclr(j,i));
        }
    }

    for(int i=SCR_H-lines; i<SCR_H; i++){
        for(int j=0; j<SCR_W; j++){
            putchr(j,i,' ');
            putclr(j,i,CLR_DEFAULT);
        }
    }

    g_screeny-=lines;
}

// =====================================

#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPECIFIER      4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4

void printf_unsigned(unsigned long long number, int radix);
void printf_signed(long long number, int radix);

void printf(const char* fmt, ...){
    va_list args;
    va_start(args, fmt);

    int state=PRINTF_STATE_NORMAL;
    int length=PRINTF_LENGTH_DEFAULT;
    bool inc=true;
    int radix=10;
    bool sign=false;
    bool number=false;

    while(*fmt){

        switch(state){
            case PRINTF_STATE_NORMAL:
                inc=true;
                switch(*fmt){
                    case '%':
                        state=PRINTF_STATE_LENGTH;
                        break;

                    default:
                        putc(*fmt);
                        break;
                }

                break;
            
            case PRINTF_STATE_LENGTH:
                inc=true;
                switch(*fmt){
                    case 'h':
                        length=PRINTF_LENGTH_SHORT;
                        state=PRINTF_STATE_LENGTH_SHORT;
                        break;

                    case 'l':
                        length=PRINTF_LENGTH_LONG;
                        state=PRINTF_STATE_LENGTH_LONG;
                        break;

                    default:
                        inc=false;
                        state=PRINTF_STATE_SPECIFIER;
                        break;
                }

                break;

            case PRINTF_STATE_LENGTH_SHORT:
                inc=true;
                if(*fmt=='h'){
                    length=PRINTF_LENGTH_SHORT_SHORT;
                }else{
                    inc=false;
                }
                state=PRINTF_STATE_SPECIFIER;
                break;

            case PRINTF_STATE_LENGTH_LONG:
                inc=true;
                if(*fmt=='l'){
                    length=PRINTF_LENGTH_LONG_LONG;
                }else{
                    inc=false;
                }
                state=PRINTF_STATE_SPECIFIER;
                break;

            case PRINTF_STATE_SPECIFIER:
                inc=true;
                switch(*fmt){
                    case 'c':
                        putc((char)va_arg(args, int));
                        break;

                    case 's':
                        
                        puts(va_arg(args, const char*));
                        break;
    
                    case '%':
                        putc('%');
                        break;
                        
                    case 'd':
                    case 'i':
                        radix=10;
                        sign=true;
                        number=true;
                        break;

                    case 'u':
                        radix=10;
                        sign=false;
                        number=true;
                        break;

                    case 'x':
                    case 'X':
                    case 'p':
                        radix=16;
                        sign=false;
                        number=true;
                        break;

                    case 'o':
                        radix=8;
                        sign=false;
                        number=true;
                        break;

                    default: 
                        break;
                }

                if(number){
                    if(sign){
                        
                        switch(length){
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:
                                printf_signed(va_arg(args, int), radix);
                                break;

                            case PRINTF_LENGTH_LONG:
                                printf_signed(va_arg(args, long), radix);
                                break;

                            case PRINTF_LENGTH_LONG_LONG:
                                printf_signed(va_arg(args, long long), radix);
                                break;
                        }
                    }else{
                        
                        switch(length){
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:
                                printf_unsigned(va_arg(args, unsigned int), radix);
                                break;

                            case PRINTF_LENGTH_LONG:
                                printf_unsigned(va_arg(args, unsigned long), radix);
                                break;

                            case PRINTF_LENGTH_LONG_LONG:
                                printf_unsigned(va_arg(args, unsigned long long), radix);
                                break;
                        }
                    }
                }

                state=PRINTF_STATE_NORMAL;
                length=PRINTF_LENGTH_DEFAULT;
                radix=10;
                sign=false;
                number=false;
                break;

        }

        fmt+=inc;
    }

    va_end(args);
}

const char* hex_chars="0123456789abcdef";

void printf_unsigned(unsigned long long number, int radix){
    char buffer[42];
    int pos=0;

    do{
        unsigned long long rem=number%radix;
        number/=radix;

        buffer[pos++]=hex_chars[rem];
    }while(number>0);


    //print in reverse order
    while(--pos >=0){
        putc(buffer[pos]);
    }
}

void printf_signed(long long number, int radix){
    if(number<0){
        putc('-');
        printf_unsigned(-number, radix);
    }else{
        printf_unsigned(number, radix);
    }
}