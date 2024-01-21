;  FILENAME :    	kernel_entry.asm
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
;   This is the beginning of our OS. Here we arrive when calling KERNEL_OFFSET
;   from our bootloader
;   This file will be linked at the beginning of kernel.bin, right before the code of kernel.c
;   This makes sure, that however kernel.c is structured, when we start execution of kernel.bin
;   we will call the main function to start execution of the os.  
[bits 32]
; declaring main so the linker can substitute it with the final adress
[extern kernel_main]   
[global _start]
_start:

; Ensure that we jump straight into the kernel's entry function. We will not return from this call
call kernel_main
; we will not reach this code
jmp  $

%include "idt.asm"
%include "isr.asm"
%include "irq.asm"