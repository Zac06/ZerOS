#include "stdint.h"

uint32_t align(uint32_t number, uint32_t align_to){
    if(!align_to){
        return number;
    }
    uint32_t remainder=number%align_to;
    
    if(remainder>0){
        return number+align_to-remainder;
    }
    return number;
}
