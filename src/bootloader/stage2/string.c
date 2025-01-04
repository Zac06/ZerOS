#include "string.h"
#include "memdefs.h"
#include "stdint.h"

unsigned strlen(const char* str){
    unsigned len=0;
    
    while(*str){
        str++;
        len++;
    }

    return len;
}

const char* strchr(const char* str, char c){
    if(str==NULL){
        return NULL;
    }

    while(*str){
        if(*str==c){
            return str;
        }
        str++;
    }

    return NULL;
}

char* strcpy(char* dst, const char* src){
    char* dst_save=dst;

    if(dst==NULL){
        return NULL;
    }

    if(src==NULL){
        *dst='\0';
        return dst;
    }

    while(*src){
        *dst=*src;
        dst++;
        src++;
    }
    *dst='\0';
    
    return dst_save;
}
