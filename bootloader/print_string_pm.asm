;  FILENAME :    	print_string_pm.asm
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
;   Function for printing in 32-bit protected mode.
;   the funtion for 16-bit real mode won't work in 32-bit PM
;   the string will alway be printed to the begining of video memory,
;   so to the top left of the screen.
;   The function will only be used once for debugging purposes
;   argument: ebx - address of 0 terminated string

[bits 32]
print_string_pm:
    pusha
    ; set edx to start of video memory
    mov edx, VIDEO_MEMORY
    ; store attributes for printing in ah
    mov ah,  WHITE_ON_BLACK

print_string_pm_loop:
    ; mov the char at ebx to al
    mov al,    [ebx]
    ; if al=0, end of string
    cmp al,    0
    je  print_string_pm_done
    ; store char and attributes in memory cell
    mov [edx], ax
    ; move to next char in string  
    add ebx,   1
    ; move to next char cell in memory
    add edx,   2

    jmp print_string_pm_loop

print_string_pm_done:
    popa
    ret

; constants
VIDEO_MEMORY   equ 0x000b8000
WHITE_ON_BLACK equ 0x0f