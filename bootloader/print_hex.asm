;  FILENAME :    	print_hex.asm
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
;   Function for printing hex numbers in 16-bit real mode
;   argument: dx - hex value to be printed
;   This would have been easier with more functions,
;   but I wanted to keep the amount of instructions low
;   to keep the bootloader size under 512 bit

print_hex:
    pusha
    ; First we want to convert hex values in dx into chars in HEX_OUT,
    ; then print HEX_OUT as a string using print_string
    
    ; iterating over the 4 digits of the hex value
    mov cx, 4
    .loop_label:
        ; move argument to ax (the value in dx changes with every loop)
        mov ax,        dx
        ; set everything except the last 4 bits to 0 to only get the last hex digit
        and ax,        0000000000001111b
        ; get the address of the HEX_DIGITS table into bx
        mov bx,        HEX_DIGITS
        ; increment bx to the adress in the table with the correct char 
        add bx,        ax
        ; move the char to al
        mov al,        [bx]
        ; move the adress of HEX_OUT into bx
        mov bx,        HEX_OUT
        ; increment bx to point to the correct digig of HEX_OUT. In the fist loop it wil be
        ; the last digit, then the second to last ...
        inc bx
        add bx,        cx
        ; move the char into the chosen digit
        mov byte [bx], al
        ; shift the input 4 bits to the right to get the next digit into the last place
        shr dx,        4
    loop .loop_label

    ; print HEX_OUT
    mov  bx, HEX_OUT
    call print_string
    popa
    ret

; global variables
HEX_OUT:    db '0x0000', 0
HEX_DIGITS: db "0123456789abcdef"