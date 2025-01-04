#include "memory.h"

void far* memcpy(void far* dst, const void far* src, uint16_t num){
    uint8_t far* u8dst=(uint8_t far*) dst;
    const uint8_t far* u8src=(const uint8_t far*) src;

    for(uint16_t i=0; i<num; i++){
        u8dst[i]=u8src[i];
    }

    return dst;
}

void far* memset(void far* ptr, int value, uint16_t size){
    uint8_t far* u8ptr=(uint8_t far*) ptr;

    for(uint16_t i=0; i<size; i++){
        u8ptr[i]=(uint8_t)value;
    }

    return ptr;
}

int memcmp(const void far* one, const void far* two, uint16_t size){
    const uint8_t far* u8one=(const uint8_t far*) one;
    const uint8_t far* u8two=(const uint8_t far*) two;

    for(uint16_t i=0; i<size; i++){
        if(u8one[i]!=u8two[i]){
            return 1;
        }
    }

    return 0;
}

