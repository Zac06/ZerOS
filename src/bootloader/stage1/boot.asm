org 0x7C00
bits 16

%define ENDL 0x0d, 0x0a     
;line feed (LF) and carriage return (CR)

;=========================================================================================

; SETTING UP FAT12 HEADER
jmp short start
nop

bdb_oem:                            db "MSWIN4.1"       ;8B
bdb_bytes_per_sector:               dw 512              ;2B
bdb_sectors_per_cluster:            db 1                ;1B
bdb_reserved_sectors:               dw 1                ;2B
bdb_fat_count:                      db 2                ;1B
bdb_dir_entries_count:              dw 0x0E0            ;2B
bdb_total_sectors:                  dw 2880             ;2B (2880*512=1.44MB of disk size)
bdb_media_descriptor_type:          db 0x0F0            ;1B ;3.5" floppy
bdb_sectors_per_fat:                dw 9                ;2B ;9 sectors/fat
bdb_sectors_per_track:              dw 18               ;2B
bdb_heads:                          dw 2                ;2B
bdb_hidden_sectors:                 dd 0                ;4B
bdb_large_sector_count:             dd 0                ;4B

;extended boot record
ebr_drive_number:                   db 0                ;1B ; floppy diskù
                                    db 0                ;1B; reserved
ebr_signature:                      db 0x29             ;1B
ebr_volume_id:                      dd 0x12345678       ;4B; should mean "serial number" (doesn't mean anything)
ebr_volume_label:                   db "ZEROS ZAC06"    ;11B (padded with spaces)
ebr_system_id:                      db "FAT12   "       ;8B (padded with spaces)

;=========================================================================================

;Code execution starts from here

;=========================================================================================

start:
    ;setup data segments

    mov ax, 0
    mov ds, ax
    mov es, ax              ;cannot do mov es/ds, 0 directly

    mov ss, ax
    mov sp, 0x7c00          ;stack grows down from the start of the program

    ;some BIOSes might start at 7C00:0000 instead of 0000:7C00
    push es
    push word .after
    retf

.after:


    ;read from floppy
    ;dl should be set to the drive number 
    mov [ebr_drive_number], dl

    mov si, msg_loading
    call puts
    
    ;read drive parameters
    push es
    mov ah, 0x08
    int 0x13
    jc floppy_error
    pop es

    and cl, 0x3f            ;remove top 2 bits
    mov ch, 0
    mov [bdb_sectors_per_track], cx

    inc dh
    mov [bdb_heads], dh     ;head count

    ;read FAT root directory
    mov ax, [bdb_sectors_per_fat]
    mov bl, [bdb_fat_count]
    mov bh, 0
    mul bx                              ;ax=fats*sectors_per_fat
    add ax, [bdb_reserved_sectors]      ;ax=LBA of root
    push ax

    ;calc. size of root dir(32*number of entries)/bytes_per_sector
    mov ax, [bdb_sectors_per_fat]
    shl ax, 5                           ;ax*32 (left shift of 5 bytes, since 2^5=32)
    mov dx, 0
    div word [bdb_bytes_per_sector]

    test dx, dx                         ;if dx!=0, add 1
    jz .root_dir_after
    inc ax                              ;div remainder!=0, add 1. This means we have a sector partially loaded with data.


.root_dir_after:

    ;read root dir
    mov cl, al                      ;cl=number of sectors to read=size of root dir
    pop ax                          ;ax=LBA of root dir
    mov dl, [ebr_drive_number]      ;dl=drive number
    mov bx, buffer                  ;es:bx=buffer
    call disk_read

    ;search for stage2
    mov bx, 0
    mov di, buffer

.search_stage2:
    mov si, stage2_filename
    mov cx, 11                      ;standard lenght of FAT filename
    push di
    repe cmpsb                      ;compare strings at ds:si and es:di decrementing cx every time
                                    ;loop goes on until cx=0 or bytes not equal
    pop di
    je .found_stage2

    add di, 32                      ;next dir entry
    inc bx                          ;increment checked directories
    cmp bx, [bdb_dir_entries_count] ;until there are no more directories to check
    jl .search_stage2               ;go back to loop

    ;stage2 not found
    jmp stage2_not_found_error

.found_stage2:
    ;di should point to dir entry
    mov ax, [di+26]                 ;first logical cluster field (offset 26)
    mov [stage2_cluster], ax

    ;load FAT into memory
    mov ax, [bdb_reserved_sectors]
    mov bx, buffer
    mov cl, [bdb_sectors_per_fat]
    mov dl, [ebr_drive_number]
    call disk_read

    ;read stage2 and process FAT chain
    mov bx, STAGE2_LOAD_SEGMENT
    mov es, bx                      ;can't mov into es directly
    mov bx, STAGE2_LOAD_OFFSET

.load_stage2_loop:
    ;read next cluster
    mov ax, [stage2_cluster]
    add ax, 31

    mov cl, 1
    mov dl, [ebr_drive_number]
    call disk_read
    
    add bx, [bdb_bytes_per_sector]

    ;calc. location of next cluster
    mov ax, [stage2_cluster]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx                      ;ax=index of FAT entry, dx=cluster%2

    mov si, buffer
    add si, ax 
    mov ax, [ds:si]             ;read from FAT at ax

    or dx, dx
    jz .even

.add:
    shr ax, 4
    jmp .next_cluster_after

.even:
    and ax, 0x0fff

.next_cluster_after:
    cmp ax, 0x0ff8              ;end of chain
    jae .read_finish

    mov [stage2_cluster], ax
    jmp .load_stage2_loop

.read_finish:
    ;jmp to stage2 (pass execution to it)
    mov dl, [ebr_drive_number]

    ;set segment registers
    mov ax, STAGE2_LOAD_SEGMENT
    mov ds, ax
    mov es, ax

    jmp STAGE2_LOAD_SEGMENT:STAGE2_LOAD_OFFSET

    ;shouldn't happen? just in case
    jmp wait_key_and_reboot

    cli
    hlt

;===============================================================================

floppy_error:
    mov si, msg_read_failed
    call puts
    jmp wait_key_and_reboot

;===============================================================================

wait_key_and_reboot:
    mov ah, 0
    int 0x16            ;wait for key
    jmp 0xFFFF:0        ;jump to beginning of BIOS, should reboot

    .halt:
        cli                 ;disable interrupts
        hlt

;===============================================================================

stage2_not_found_error:
    mov si, msg_stage2_nf
    call puts
    jmp wait_key_and_reboot

;===============================================================================

;prints a string to the screen.
;parameters: ds:si to string.
puts:
    push si
    push ax
    push bx

    .puts_loop:
        lodsb                   ;load char in AL (8 bits, hence ASCII)
        or al, al               ;check if null: if 0flag is set, then it's null
        jz .puts_done

        mov ah, 0x0e
        mov bh, 0
        int 0x10

        jmp .puts_loop

    .puts_done:
        pop bx
        pop ax
        pop si
        ret                     ;return from where it was called


;===================================================================================

;Disk routines

;=================================

;lba address to chs address
;params:    ax - LBA address
;returns:   cx - sector (bits 0-5)
;           cx - cylinder (bits 6-15)
;           dh - head
lba_to_chs:
    push ax
    push dx

    mov dx, 0
    div word [bdb_sectors_per_track]        ;ax = LBA / sectors_per_track
                                            ;dx = LBA % sectors_per_track
    inc dx
    mov cx, dx                              ;cx=sector
    
    mov dx, 0
    div word [bdb_heads]                    ;ax = LBA / sectors_per_track / heads = cylinder

    mov dh, dl                              ;dh = head
    mov ch, al                              ;ch = cylinder

    shl ah, 6                               ;shift left by 6 bits
    or cl, ah                               ;put upper 2 bits of cylinder in cl

    pop ax
    mov dl, al
    pop ax

    ret

;read sectors_from_disk
;params:    ax = LBA address
;           cl = num sectors to read (max 128)
;           es:bx = where to store resulting data

disk_read:
    push ax
    push bx
    push cx
    push dx
    push di

    push cx
    call lba_to_chs
    pop ax                                  ;al = number of sectors to read
    
    mov ah, 0x2
    mov di, 3                               ;number of read tries to perform

    .retry_read:
        pusha                               ;don't know what bios sets so save everything
        stc                                 ;set carry flag
        int 0x13                            ;carry flag clear = success
        jnc .done_read

        ;if reach here, read failed
        popa

        call disk_reset
        dec di
        test di, di                         ;if di is 0
        jnz .retry_read

    .fail_read:
        ;if reach here, all read tries failed
        jmp floppy_error

    .done_read:
        popa

        pop di
        pop dx
        pop cx
        pop bx
        pop ax

        ret

;resets disk controller
;params:    dl = drive number
disk_reset:
    pusha
    mov ah, 0
    stc
    int 0x13
    jc floppy_error
    popa
    ret

;======================================================================================

msg_loading:        db "Loading...", ENDL, 0
msg_read_failed:    db "Floppy read failed!", ENDL, 0
msg_stage2_nf:      db "stage2 not found!", ENDL, 0

stage2_filename:    db "STAGE2  BIN"
stage2_cluster:     dw 0

STAGE2_LOAD_SEGMENT     equ 0x0      ;equ: don't allocate memory for the constant. basically, a #define in C
STAGE2_LOAD_OFFSET      equ 0x500

times 510-($-$$) db 0       ;DIRECTIVE: fill the program with 0s for the first sector (512 bytes)
dw 0AA55h                   ;

buffer: