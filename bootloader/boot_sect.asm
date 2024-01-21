;  FILENAME :    	boot_sect.asm
;
;  AUTHOR :      	Ruben Lohberg
;
;  START DATE:   	22 Oct 2023
;
;  LAST UPDATE: 	21 Jan 2024
;
;  PROJECT:   	  RubenOS
;
;  DESCRIPTION:
;   This is the beginning of the bootloader. From here we will
;   switch the processor from the default 16-bit real mode
;   to 32-bit protected mode and load the c kernel
;   0x7c00 is the adress where BIOS puts the 512bit bootloader


; all absolute adresses will be offset by 0x7c00
[org 0x7c00]    

    ; the BIOS stored the drive number of our boot drive in the register 
    ; dl for us. We load this value into memory under the variable BOOT_DRIVE
    mov [BOOT_DRIVE], dl
    
    ; move the stack pointer far enough away, so it won't grow into our kernel code
    mov bp, 0xe000
    mov sp, bp

    ; announce our start in 16-bit real mode
    mov  bx, MSG_REAL_MODE
    call print_string

    ; loading the kernel to a meory offset we define
    call load_kernel

    KERNEL_OFFSET_ADDR equ 0xf000

    ; switch to 32 bit protected mode, from which we will not return
    call switch_to_pm

    ; endlessly jump to current address, this code will not be reached
    jmp $

; self written routines
%include "print_string.asm"
%include "disk_load.asm"
%include "print_hex.asm"
%include "gdt.asm"
%include "print_string_pm.asm"
%include "switch_to_pm.asm"

[bits 16]

load_kernel:
    ; print message to announce that we are loading the kernel
    mov  bx, MSG_LOAD_KERNEL
    call print_string

    ; loading the kernel from the boot drive to our defined offset
    mov  dl, [BOOT_DRIVE]
    mov  bx, KERNEL_OFFSET_ADDR
    call disk_load
ret

[bits 32]
; this is where we arrive after switiching to 32 bit protected mode
BEGIN_PM:
    ; announce our arrival in 32 bit pm
    mov  ebx, MSG_PROT_MODE
    call print_string_pm

    ; Now we jump to the kernel code. Again, we will not return from this call
    ; We will arrive at the beginning of kernel_entry.asm where our OS begins
    call KERNEL_OFFSET_ADDR

jmp $

; GLOBAL variables
BOOT_DRIVE       db 0
MSG_REAL_MODE    db "Started in 16 bit RM. ",0
MSG_PROT_MODE    db "Sucessfully landed in 32-bit PM. ", 0
MSG_LOAD_KERNEL  db "Loading kernel into memory. ", 0

; padding zeroes to get our bootloader to the size of 512 byte (510 byte + magic
; number)
times 510-($-$$) db 0

; magic number for boot sector being the last two bytes 
dw                  0xaa55
