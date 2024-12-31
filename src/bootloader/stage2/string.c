#include "string.h"

uint16_t strlen(const char* str){
    uint16_t len=0;
    
    while(*str){
        str++;
        len++;
    }

    return len;
}