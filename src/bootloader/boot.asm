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
    jmp main                ;make it so that the program always starts from main:

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

;================================================================================

floppy_error:
    mov si, msg_read_failed
    call puts
    jmp wait_key_and_reboot
    
    wait_key_and_reboot:
        mov ah, 0
        int 0x16            ;wait for key
        jmp 0xFFFF          ;jump to beginning of BIOS, should reboot

    .halt:
        cli                 ;disable interrupts
        hlt

;===============================================================================

main:
    ;setup data segments

    mov ax, 0
    mov dx, 0
    mov es, ax              ;cannot do mov es/ds, 0 directly

    mov ss, ax
    mov sp, 0x7c00          ;stack grows down from the start of the program

    ;read from floppy
    ;dl should be set to the drive number 
    mov [ebr_drive_number], dl
    mov ax, 1               ;LBA address
    mov cl, 1               ;read 1 sector
    mov bx, 0x7e00          ;put data after bootloader
    call disk_read

    ;print msg
    mov si, msg_akunamatata
    call puts

    cli
    hlt


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

msg_akunamatata: db "akunamatataragazzi", ENDL, 0
msg_read_failed: db "floppy read failed!", ENDL, 0

times 510-($-$$) db 0       ;DIRECTIVE: fill the program with 0s for the first sector (512 bytes)
dw 0AA55h                   ;