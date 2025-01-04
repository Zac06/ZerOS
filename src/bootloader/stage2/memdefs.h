#pragma once

#define NULL (void*) 0x0

// 0x00000000 - 0x000003FF (interrupt vector table)
// 0x00000400 - 0x000004FF (bios data)

#define MEM_MIN         0x00000500
#define MEM_MAX         0x00080000

// 0x00000500 - 0x00010500 (fat driver)
#define MEM_FAT_ADDR    ((void far*) 0x00500000)        //64 kb fat memory
#define MEM_FAT_SIZE    0x00010000                      //NOTE: USE LITTLE ENDIAN NOTATION (bytes reversed)

// 0x00030000 - 0x0007FFFF (free)

// 0x00080000 - 0x0009FFFF (extended bios data)
// 0x000A0000 - 0x000C7FFF (video)
// 0x000C8000 - 0x000FFFFF (bios)
