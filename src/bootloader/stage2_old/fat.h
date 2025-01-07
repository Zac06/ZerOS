#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "disk.h"
#include "memdefs.h"

//enable packing from now on
#pragma pack(push, 1)

typedef struct {
    uint8_t name[11];
    uint8_t attributes;
    uint8_t _reserved;
    uint8_t created_time_tenths;
    uint16_t created_time;
    uint16_t creation_date;
    uint16_t accessed_date;
    uint16_t first_cluster_high;
    uint16_t modified_time;
    uint16_t modified_date;
    uint16_t first_cluster_low;
    uint32_t size;

} fat_direntry;

//disable packing from now on
#pragma pack(pop)

typedef struct {
    int handle;
    bool is_dir;
    uint32_t pos;    //in sectors
    uint32_t size;
} fat_file;         //file handler

enum fat_attrs {
    FAT_ATTR_RO         =0x01,
    FAT_ATTR_HID        =0x02,
    FAT_ATTR_SYS        =0x04,
    FAT_ATTR_VOL_ID     =0x08,
    FAT_ATTR_DIR        =0x10,
    FAT_ATTR_ARCH       =0x20,
    FAT_ATTR_LFN        =FAT_ATTR_RO | FAT_ATTR_HID | FAT_ATTR_SYS | FAT_ATTR_VOL_ID
};

bool fat_init(disk* dsk);
fat_file far* fat_open(disk* dsk, const char* path);
uint32_t fat_read(disk* dsk, fat_file far* file, uint32_t byte_cnt, void* out);
bool fat_read_entry(disk* dsk, fat_file far* file, fat_direntry* dir_entry);
void fat_close(fat_file far* file);
