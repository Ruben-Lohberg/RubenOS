;  FILENAME :    	print_string.asm
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
;   function for printing a string in 16-bit real mode
;   The string will be printed at the cursor
;   arguments: bx - adress of  0 terminated string to be printed

[bits 16]

print_string:
    pusha
    ; move argument string to dx
    mov dx, bx
    ; set teletype output option for int 10
    mov ah, 0eh
    ; set page nr 0 for teletype output
    mov bh, 0
    ; set text color=white for teletype output 
    mov bl, 0fh
    ; loop over chars
    .loop:
        ; move current char into al
        mov al, [edx]
        ; if char is a 0, jump out of loop
        cmp al, 0
        je  .done
        ; interrupt for printing char at cursor
        int 10h
        ; increment dx to advance to the next char
        inc dx
        jmp .loop
    .done:
    popa
    ret