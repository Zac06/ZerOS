#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

typedef struct {
    uint8_t boot_jmp_instruct[3];
    uint8_t oem_id[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t dir_entry_count;
    uint16_t total_sectors;
    uint8_t media_descriptor_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t large_sector_count;

    uint8_t drive_no;
    uint8_t _reserved;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_label[11];
    uint8_t system_id[8];
} __attribute__ ((packed)) boot_sector;

typedef struct {
    uint8_t name[11];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t created_time_tenths;
    uint16_t created_time;
    uint16_t creation_date;
    uint16_t accessed_date;
    uint16_t first_cluster_high;
    uint16_t modified_time;
    uint16_t modified_date;
    uint16_t first_cluster_low;
    uint32_t size;

} __attribute__((packed)) dir_entry;

boot_sector g_bs;
uint8_t* g_fat=NULL;
dir_entry* g_rootdir=NULL;  //array of directory entries
uint32_t g_rootdir_end;

bool read_boot_sector(FILE* disk){
    return fread(&g_bs, sizeof(g_bs), 1, disk) > 0;
}

bool read_sectors(FILE* disk, uint32_t lba, uint32_t count, void* buf_out){
    bool ok=true;
    ok=ok&&(fseek(disk, lba*g_bs.bytes_per_sector, SEEK_SET)==0);
    ok=ok&&(fread(buf_out, g_bs.bytes_per_sector, count, disk)==count);
    return ok;
}

bool read_fat(FILE* disk){
    g_fat=(uint8_t*)malloc(g_bs.sectors_per_fat*g_bs.bytes_per_sector);
    return read_sectors(disk, g_bs.reserved_sectors, g_bs.sectors_per_fat, g_fat);
}

bool read_root_dir(FILE* disk){
    uint32_t lba=g_bs.reserved_sectors+g_bs.sectors_per_fat*g_bs.fat_count;
    uint32_t size=sizeof(dir_entry)*g_bs.dir_entry_count;
    uint32_t sectors=(size/g_bs.bytes_per_sector);
    if(size%g_bs.bytes_per_sector>0){
        sectors++;
    }

    g_rootdir_end=sectors+lba;
    g_rootdir=(dir_entry*)malloc(sectors*g_bs.bytes_per_sector);
    return read_sectors(disk, lba, sectors, g_rootdir);
}

dir_entry* find_file(const char* name){
    for(uint32_t i=0; i<g_bs.dir_entry_count; i++){
        if(memcmp(name, g_rootdir[i].name, 11)==0){
            return &g_rootdir[i];
        }
    }
    return NULL;
}

bool read_file(dir_entry* file_entry, FILE* disk, uint8_t* out_buf){
    uint16_t current_cluster=file_entry->first_cluster_low;
    bool ok=true;

    do {
        uint32_t lba=g_rootdir_end+(current_cluster-2)*g_bs.sectors_per_cluster;
        ok=ok&&read_sectors(disk, lba, g_bs.sectors_per_cluster, out_buf);
        out_buf+=g_bs.sectors_per_cluster*g_bs.bytes_per_sector;

        uint32_t fat_index=current_cluster*3/2;
        if(!current_cluster%2){
            current_cluster=(*(uint16_t*)(g_fat+fat_index)) & 0x0FFF;
        }else{
            current_cluster=(*(uint16_t*)(g_fat+fat_index))>>4;
        }

    }while(ok&&current_cluster<0x0FF8);
}

int main(int argc, char** argv){
    if(argc<3){
        fprintf(stderr, "Incorrect number of parameters. Syntax: %s <disk image> <file name>\n", argv[0]);
        return -1;
    }

    FILE* disk=fopen(argv[1], "rb");
    if(!disk){
        fprintf(stderr, "Cannot open disk image (%s)!\n", argv[1]);
        return -1;
    }

    if(!read_boot_sector(disk)){
        fprintf(stderr, "could not read the boot sector!\n");
        fclose(disk);
        return -2;
    }

    if(!read_fat(disk)){
        fprintf(stderr, "could not read FAT!\n");
        free(g_fat);
        fclose(disk);
        return -3;
    }

    if(!read_root_dir(disk)){
        fprintf(stderr, "could not read root directory!\n");
        free(g_fat);
        free(g_rootdir);
        fclose(disk);
        return -4;
    }

    dir_entry* file_entry=find_file(argv[2]);
    if(!file_entry){
        fprintf(stderr, "could not find the file%s!\n", argv[2]);
        free(g_fat);
        free(g_rootdir);
        return -5;
    }

    uint8_t* buffer=(uint8_t*)malloc(file_entry->size+g_bs.bytes_per_sector);
    if(!read_file(file_entry, disk, buffer)){
        fprintf(stderr, "Could not read file %s\n!", argv[2]);
        free(buffer);
        free(g_fat);
        free(g_rootdir);
        return -6;
    }

    for(size_t i=0; i<file_entry->size; i++){
        if(isprint(buffer[i])){
            fputc(buffer[i], stdout);
        }else{
            printf("<%02x", buffer[i]);
        }
    }
    printf("\n");

    free(g_fat);
    free(g_rootdir);
    fclose(disk);
}