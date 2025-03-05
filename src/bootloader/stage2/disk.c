#include "disk.h"
#include "x86.h"

#include "stdint.h"
#include "stdbool.h"

#define DISK_READ_ATTEMPTS      3

bool disk_init(disk* dsk, uint8_t drive_no){
    uint8_t drive_type;
    uint16_t cylinders, sectors, heads;

    if(!x86_disk_getdriveparams(drive_no, &drive_type, &cylinders, &sectors, &heads)){
        return false;
    }

    dsk->id=drive_no;
    dsk->cylinders=cylinders;
    dsk->heads=heads;
    dsk->sectors=sectors;
    
    return true;
}

void disk_lba2chs(disk* dsk, uint32_t lba, uint16_t* cyl_out, uint16_t* sect_out, uint16_t* heads_out){
    /**
     * sector=(lba % sectors_per_track)+1
     * cylinder=(lba / sectors_per_track) / heads
     * head=(lba / sectors_per_track) % heads
     */

    *sect_out=lba%dsk->sectors+1;
    *cyl_out=(lba/dsk->sectors)/dsk->heads;
    *heads_out=(lba/dsk->sectors)%dsk->heads;
}

bool disk_read_sectors(disk* dsk, uint32_t lba, uint8_t sectors, void* lower_data_out){
    uint16_t cylinder, sector, head;
    
    disk_lba2chs(dsk, lba, &cylinder, &sector, &head);

    for(int i=0; i<DISK_READ_ATTEMPTS; i++){
        if(x86_disk_read(dsk->id, cylinder, sector, head, sectors, lower_data_out)){
            return true;
        }
        x86_disk_reset(dsk->id);
    }

    return false;
}

