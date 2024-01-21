;  FILENAME :    	disk_load.asm
;
;  AUTHOR :      	Ruben Lohberg
;
;  START DATE:   	24 Oct 2023
;
;  LAST UPDATE: 	21 Jan 2024
;
;  PROJECT:   	  RubenOS
;
;  DESCRIPTION:
;   load sectors to ES:BX from drive dl (drive nr)

disk_load:
    pusha
    ; load 128 setors. This is the maximum number and the kernel
    ; will never reach that size. Instead the BIOS will load
    ; as many sector as possible and an error will occur.
    ; This is okay for us, because our sectors will be loaded.
    ; There might be a better way to do this, by knowing the size
    ; of the os and loading the according amount of sectors.
    mov dh, 128
    ; BIOS read sector funtion for int 13h
    mov ah, 0x02
    ; set amount of sectors to read
    mov al, dh
    ; select cylinder 0
    mov ch, 0x00
    ; select head 0
    mov dh, 0x00
    ; start reading from sector 2 (i.e. after boot sector)
    mov cl, 0x02
    ; BIOS interrupt
    int 13h

    ; debugging print
    call print_sectors_loaded
    
    popa
    ret

print_sectors_loaded:
    mov  bx, SECTORS_LOADED_MSG
    call print_string
    ; because an error occurred while trying to read 128 secors,
    ; the amount of actually read sectors is in ax. We print
    ; this number for debugging purposes
    mov  dx, ax
    mov  dh, 0
    call print_hex
    cli
ret

; Variables
SECTORS_LOADED_MSG db "Disk: Sectors loaded: ", 0