#include "x86.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

void putc(char c){
    x86_video_writechar_tty(c, 0);
}

void puts(const char* str){
    x86_video_writestr_tty(str, strlen(str), 0, 0x07);
}

void puts_f(const char far* str){
    while(*str){
        putc(*str);
        str++;
    }
}

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

int* printf_number(int* argp, int length, bool sign, int radix);

void _cdecl printf(const char* fmt, ...){
    int state=PRINTF_STATE_NORMAL;
    int* argp=(int*)&fmt;
    int length=PRINTF_LENGTH_DEFAULT;
    bool inc=true;
    int radix=10;
    bool sign=false;

    argp++;                         //point to the 2nd argument

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
                        putc((char)*argp);
                        argp++;
                        break;

                    case 's':
                        if(length==PRINTF_LENGTH_LONG || length==PRINTF_LENGTH_LONG_LONG){
                            puts_f(*(const char far**)argp);
                            argp+=2;
                        }else{
                            puts_f(*(const char**)argp);
                            argp++;
                        }
                        
                        break;

                    case '%':
                        putc('%');
                        break;

                    case 'd':
                    case 'i':
                        radix=10;
                        sign=true;
                        argp=printf_number(argp, length, sign, radix);
                        break;

                    case 'u':
                        radix=10;
                        sign=false;
                        argp=printf_number(argp, length, sign, radix);
                        break;

                    case 'x':
                    case 'X':
                    case 'p':
                        radix=16;
                        sign=false;
                        argp=printf_number(argp, length, sign, radix);
                        break;

                    case 'o':
                        radix=8;
                        sign=false;
                        argp=printf_number(argp, length, sign, radix);
                        break;

                    default: 
                        break;
                }

                state=PRINTF_STATE_NORMAL;
                length=PRINTF_LENGTH_DEFAULT;
                radix=10;
                sign=false;

                break;

        }

        fmt+=inc;
    }
}

const char* hex_chars="0123456789abcdef";

int* printf_number(int* argp, int length, bool sign, int radix){
    char buffer[42];
    uint64_t number;
    int number_sign=1;
    int pos=0;

    switch(length){
        case PRINTF_LENGTH_SHORT_SHORT:
        case PRINTF_LENGTH_SHORT:
        case PRINTF_LENGTH_DEFAULT:
            if(sign){
                int n=*argp;
                if(n<0){
                    n=-n;
                    number_sign=-1;
                }
                number=(unsigned long long)n;
            }else{
                number=*(unsigned int*)argp;
            }
            argp++;

            break;

        case PRINTF_LENGTH_LONG:
            if(sign){
                long int n=*(long int*)argp;
                if(n<0){
                    n=-n;
                    number_sign=-1;
                }
                number=(unsigned long long)n;
            }else{
                number=*(unsigned long int*)argp;
            }
            argp+=2;
            
            break;

        case PRINTF_LENGTH_LONG_LONG:
            if(sign){
                long long int n=*(long long int*)argp;
                if(n<0){
                    n=-n;
                    number_sign=-1;
                }
                number=(unsigned long long)n;
            }else{
                number=*(unsigned long long*)argp;
            }
            argp+=4;
            
            break;
    }

    do{
        uint32_t rem;
        
        x86_div64_32(number, radix, &number, &rem);

        buffer[pos++]=hex_chars[rem];
    }while(number>0);

    if(sign&&number_sign<0){
        buffer[pos++]='-';
    }

    //print in reverse order
    while(--pos >=0){
        putc(buffer[pos]);
    }

    return argp;
}
