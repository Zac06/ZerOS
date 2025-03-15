#pragma once

#include<stdint.h>

template<typename T, uint32_t N>
uint32_t arrsize(const T (&)[N]){
    return N;
}