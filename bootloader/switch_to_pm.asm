;  FILENAME :    	switch_to_pm.asm
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
;   Switch the processor to 32-bit protected mode

[bits 16]

switch_to_pm:
    ; clear interrupts
    cli                 
    ; Load global descriptor table    
    lgdt [gdt_descriptor]

    ; set the first bit of cr0 to 1 for protected mode, without changing other bits,
    ; by loading the current calue into eax and using the or command. 
    ; or can not be used dirctly on cr0
    mov eax, cr0
    or  eax, 0x1
    mov cr0, eax

    ; make a far jump to our 32 bit code. This also forces to CPU to clear the pipeline of pre-fetched
    ; instructions, which were real mode instructions
    jmp CODE_SEG:init_pm
                            

[bits 32]
; We are now in 32-bit real mode.
; Initialize registers and stack in PM
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; update stack to the top of our free space
    mov ebp, 0x90000
    mov esp, ebp

    ; call back to a label in our boot_sect.asm file. We will not return from this call
    call BEGIN_PM