#include "stdint.h"
#include "stdbool.h"
#include "fat.h"
#include "stdio.h"
#include "string.h"

#include "memdefs.h"
#include "memory.h"
#include "utils.h"
#include "ctype.h"
#include "math.h"

#define SECTOR_SIZE         512
#define MAX_PATH_SIZE       256
#define MAX_FILE_HANDLES    10
#define ROOT_DIR_HANDLE     -1

#pragma pack(push, 1)

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
} fat_bootsector;

#pragma pack(pop)

typedef struct {
    fat_file public;
    bool opened;
    uint32_t first_clust;
    uint32_t curr_clust;
    uint32_t curr_sect_in_clust;
    uint8_t buffer[SECTOR_SIZE];

} fat_filedata;

typedef struct {        //temp buffers
    union {
        fat_bootsector boot_sec;
        uint8_t bs_bytes[SECTOR_SIZE];
    } bs;

    fat_filedata root_dir;
    fat_filedata opened_files[MAX_FILE_HANDLES];

} fat_data;

static fat_data far* g_data;        //make sure things are loaded immediately at runtime
static uint8_t far* g_fat=NULL;
static uint32_t g_data_section_lba;
//fat_direntry* g_rootdir=NULL;  //array of directory entries
//uint32_t g_rootdir_end;

bool fat_read_bootsector(disk* dsk){
    return disk_read_sectors(dsk, 0, 1, g_data->bs.bs_bytes);
}

bool fat_read_fat(disk* dsk){
    return disk_read_sectors(dsk, g_data->bs.boot_sec.reserved_sectors, g_data->bs.boot_sec.sectors_per_fat, g_fat);
}

bool fat_init(disk* dsk){
    g_data=(fat_data far*) MEM_FAT_ADDR;

    if(!fat_read_bootsector(dsk)){
        printf("error reading bootsector from drive %d\r\n", dsk->id);      //using CR LF
        return false;
    }

    //load FAT into memory
    g_fat=(uint8_t far*) g_data+sizeof(fat_data);
    uint32_t fat_size=g_data->bs.boot_sec.bytes_per_sector*g_data->bs.boot_sec.sectors_per_fat;
    
    if(sizeof(fat_data)+fat_size>=MEM_FAT_SIZE){
        printf("FAT of drive %d is too big. Required %lu, while available is %lu\r\n", dsk->id, sizeof(fat_data)+fat_size, MEM_FAT_SIZE);
        return false;
    }

    if(!fat_read_fat(dsk)){
        printf("Cannot read FAT of drive %d", dsk->id);
        return false;
    }

    uint32_t rootdir_size=sizeof(fat_direntry)*g_data->bs.boot_sec.dir_entry_count;
    uint32_t rootdir_lba=g_data->bs.boot_sec.reserved_sectors+g_data->bs.boot_sec.sectors_per_fat*g_data->bs.boot_sec.fat_count;

    //open root dir
    g_data->root_dir.opened=true;
    g_data->root_dir.public.handle=ROOT_DIR_HANDLE;
    g_data->root_dir.public.pos=0;
    g_data->root_dir.public.size=sizeof(fat_direntry)*g_data->bs.boot_sec.dir_entry_count;
    g_data->root_dir.public.is_dir=true;
    g_data->root_dir.first_clust=rootdir_lba;
    g_data->root_dir.curr_clust=rootdir_lba;
    g_data->root_dir.curr_sect_in_clust=0;

    if(!disk_read_sectors(dsk, rootdir_lba, 1, g_data->root_dir.buffer)){
        printf("Cannot read rootdir of drive %d", dsk->id);
        return false;
    }

    //calc data section
    uint32_t rootdir_sects=(rootdir_size+g_data->bs.boot_sec.bytes_per_sector-1)/g_data->bs.boot_sec.bytes_per_sector;
    g_data_section_lba=rootdir_lba+rootdir_sects;

    //zero all file handles
    for(int i=0; i<MAX_FILE_HANDLES; i++){
        g_data->opened_files[i].opened=false;
    }

    return true;
}

uint32_t fat_clust2lba(uint32_t clust){
    return g_data_section_lba+(clust-2)*g_data->bs.boot_sec.sectors_per_cluster;
}

fat_file far* fat_open_entry(disk* dsk, fat_direntry* entry){
    int handle=-1;
    for(int i=0; i<MAX_FILE_HANDLES && handle<0; i++){
        if(!g_data->opened_files[i].opened){
            handle=i;
        }
    }

    //no handles available
    if(handle<0){
        printf("No available file handles on drive %d\r\n", dsk->id);
        return NULL;
    }

    fat_filedata far* fd=&g_data->opened_files[handle];
    fd->public.handle=handle;
    fd->public.is_dir=(entry->attributes & FAT_ATTR_DIR) !=0;
    fd->public.pos=0;
    fd->public.size=entry->size;
    fd->first_clust=entry->first_cluster_low+((uint32_t)entry->first_cluster_high<<16);
    fd->curr_clust=fd->first_clust;
    fd->curr_sect_in_clust=0;

    if(!disk_read_sectors(dsk, fat_clust2lba(fd->curr_clust), 1, fd->buffer)){
        printf("Read error on drive %d\r\n", dsk->id);
        return NULL;
    }

    fd->opened=true;
    return &fd->public;
}

uint32_t fat_next_clust(uint32_t curr_clust){
    uint32_t fatind=curr_clust*3/2;

    if(curr_clust%2==0){
        return (*(uint16_t far*)(g_fat+fatind)) & 0x0FFF;
    }
    return (*(uint16_t far*)(g_fat+fatind))>>4;
    
}

uint32_t fat_read(disk* dsk, fat_file far* file, uint32_t byte_cnt, void* out){
    //get file data
    fat_filedata far* fd;
    if((file->handle==ROOT_DIR_HANDLE)){
        fd=&g_data->root_dir;
    }else{
        fd=&g_data->opened_files[file->handle];
    }

    uint8_t* u8out=(uint8_t*)out;

    if(!fd->public.is_dir){
        /*printf("BYTE_CNT before: %d\r\n", byte_cnt);
        printf("size: %d\r\n", fd->public.size);
        printf("pos: %d\r\n", fd->public.pos);
        printf("sizeof(fat_direntry): %d\r\n", sizeof(fat_direntry));
*/
        byte_cnt=min(byte_cnt, fd->public.size-fd->public.pos);

        //printf("BYTE_CNT after: %d\r\n", byte_cnt);
    }
    

    while(byte_cnt>0){
        uint32_t left_buf=SECTOR_SIZE-(fd->public.pos%SECTOR_SIZE);
        uint32_t take=min(byte_cnt, left_buf);

        memcpy(u8out, fd->buffer+fd->public.pos%SECTOR_SIZE, take);
        u8out+=take;
        fd->public.pos+=take;
        byte_cnt-=take;

        //read more data??
        if(left_buf==take){

            //root needs special handling
            if(fd->public.handle==ROOT_DIR_HANDLE){
                ++fd->curr_clust;
                
                if(!disk_read_sectors(dsk, fd->curr_clust, 1, fd->buffer)){
                    //handle read error
                    printf("fat_read(): read error, drive no %d\r\n", dsk->id);
                    break;
                }
            }else{
                if(++fd->curr_sect_in_clust>=g_data->bs.boot_sec.sectors_per_cluster){
                    //read next cluster since we reached the end of the current one
                    fd->curr_sect_in_clust=0;
                    fd->curr_clust=fat_next_clust(fd->curr_clust);
                }

                if(fd->curr_clust>=0xff8){
                    fd->public.size=fd->public.pos;
                    break;
                }

                if(!disk_read_sectors(dsk, fat_clust2lba(fd->curr_clust)+fd->curr_sect_in_clust, 1, fd->buffer)){
                    //handle read error
                    printf("fat_read(): read error, drive no %d\r\n", dsk->id);
                    break;
                }
            }
            
        }
        
    }

    //printf("ret\r\n");

    return u8out-(uint8_t*)out;     //size of read data
}

bool fat_read_entry(disk* dsk, fat_file far* file, fat_direntry* dir_entry){
    return fat_read(dsk, file, sizeof(fat_direntry), dir_entry)==sizeof(fat_direntry);
}

void fat_close(fat_file far* file){
    if(file->handle==ROOT_DIR_HANDLE){
        file->pos=0;
        g_data->root_dir.curr_clust=g_data->root_dir.first_clust;
    }else{
        g_data->opened_files[file->handle].opened=false;
    }
}

bool fat_find_file(disk* dsk, fat_file far* file, const char* name, fat_direntry* entry_out){
    fat_direntry entry;
    char fatname[12];       //FAT names are like this: uppercase, 8 chars for name, 3 for ext, no special chars

    memset(fatname, ' ', sizeof(fatname));
    fatname[11]='\0';

    const char* ext=strchr(name, '.');
    if(ext==NULL){
        ext=name+11;
    }

    for(int i=0; i<8 && name[i] && name+i<ext; i++){
        fatname[i]=toupper(name[i]);
    }

    if(ext!=NULL){
        for(int i=0; i<3 && ext[i+1]; i++){
            fatname[i+8]=toupper(ext[i+1]);
        }
    }

    while(fat_read_entry(dsk, file, &entry)){
        if(memcmp(fatname, entry.name, 11)==0){
            *entry_out=entry;
            return true;
        }
    }

    return false;
}

fat_file far* fat_open(disk* dsk, const char* path){
    char name[MAX_PATH_SIZE];

    //ignore leading slash
    if(*path=='/'){
        path++;
    }

    //fat_file far* parent=NULL;
    fat_file far* curr=&g_data->root_dir.public;

    while(*path){
        bool is_last=false;

        const char* delim=strchr(path, '/');
        if(delim!=NULL){
            memcpy(name, path, delim-path);
            name[delim-path+1]='\0';
            path=delim+1;

        }else{
            unsigned len=strlen(path);
            memcpy(name, path, len);
            name[len+1]='\0';
            path+=len;
            is_last=true;
        }

        fat_direntry entry;
        if(fat_find_file(dsk, curr, name, &entry)){
            fat_close(curr);

            if(!is_last && entry.attributes & FAT_ATTR_DIR==0){
                printf("fat_open(): %s is not a directory\r\n", name);
                return NULL;
            }

            //parent=curr;
            curr=fat_open_entry(dsk, &entry);

        }else{
            fat_close(curr);
            printf("fat_open(): %s not found\r\n", name);
            return NULL;
        }
    }

    return curr;
}

