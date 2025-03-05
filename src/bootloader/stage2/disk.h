#pragma once

#include "x86.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t id;
    uint16_t cylinders;
    uint16_t sectors;
    uint16_t heads;
} disk;

bool disk_init(disk* dsk, uint8_t drive_no);
bool disk_read_sectors(disk* dsk, uint32_t lba, uint8_t sectors, void* lower_data_out);
