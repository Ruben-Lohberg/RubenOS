;  FILENAME :    	gdt.asm
;
;  AUTHOR :      	Nick Blundell / Ruben Lohberg
;
;  START DATE:   	2 Dec 2010
;
;  LAST UPDATE: 	21 Jan 2024
;
;  PROJECT:   	    RubenOS / Writing a simple Operating system - From Scratch
;
;  DESCRIPTION:
;   Global Descriptor table
;   this table is loaded via the "lgdt" command in "switch_to_pm"
;   for further explanation see "Writing a simple Operating system - From
;   Scratch" Chapter 4

gdt_start:

; mandatory 8 byte null descriptor at beginning
gdt_null:   
    ; 'dd' means define double word, so 4 bytes
    dd 0x0
    dd 0x0

; code segment descriptor
gdt_code:   
    ; base=0x0, limit=0xfffff
    ; 1st flags: (present)1 (privilege)00 (descriptor type)1 -> 1001b
    ; type flags: (code)1 (conforming)0 (readable)1 (accessed)0 -> 1010b
    ; 2nd flags: (granulatiry)1 (32-bit default)1 (64-bit seg)0 (AVL)0 -> 1100b
    dw 0xffff    ; Limit (bits 0-15)
    dw 0x0       ; Base (bits 0-15)
    db 0x0       ; Base (bits 16-23)
    db 10011010b ; 1st flags, type flags
    db 11001111b ; 2nd flags, limit (bits 16-19)
    db 0x0       ; Base (bits 24-31)

; data segment descriptor
gdt_data:   
    ; Same as code segment exept for type flags:
    ; type flags: (code)0 (expand down)0 (writable)1 (accessed)0 -> 0010b
    dw 0xffff    ; Limit (bits 0-15)
    dw 0x0       ; Base (bits 0-15)
    db 0x0       ; Base (bits 16-23)
    db 10010010b ; 1st flags, type flags
    db 11001111b ; 2nd flags, limit (bits 16-19)
    db 0x0       ; Base (bits 24-31)

; label at the end to allow the assembler to calculate the size of the gdt for the descriptor
gdt_end:    

; GDT descriptor
gdt_descriptor:
    ; Size of gdt, always one less than true size
    dw gdt_end - gdt_start -1
    ; Start adress
    dd gdt_start

; Defining some handy constants
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start