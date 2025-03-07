#pragma once

#include<stdint.h>

#define i686_GDT_CODE_SEGMENT   0x8         //index of segment multiplied by descriptor size (1*8)
#define i686_GDT_DATA_SEGMENT   0x10        //index of segment multiplied by descriptor size (2*8)