#include <std/string.h>
#include <stdint.h>

void* memcpy(void* dst, const void* src, uint16_t num){
    uint8_t* u8dst=(uint8_t*) dst;
    const uint8_t* u8src=(const uint8_t*) src;

    for(uint16_t i=0; i<num; i++){
        u8dst[i]=u8src[i];
    }

    return dst;
}

void* memset(void* ptr, int value, uint16_t size){
    uint8_t* u8ptr=(uint8_t*) ptr;

    for(uint16_t i=0; i<size; i++){
        u8ptr[i]=(uint8_t)value;
    }

    return ptr;
}

int memcmp(const void* one, const void* two, uint16_t size){
    const uint8_t* u8one=(const uint8_t*) one;
    const uint8_t* u8two=(const uint8_t*) two;

    for(uint16_t i=0; i<size; i++){
        if(u8one[i]!=u8two[i]){
            return 1;
        }
    }

    return 0;
}

