;  FILENAME :    	isr.asm
;
;  AUTHOR :      	Ruben Lohberg 
;
;  START DATE:   	20 Nov 2023
;
;  LAST UPDATE: 	21 Jan 2024
;
;  PROJECT:   	  RubenOS
;
;  DESCRIPTION:
;   Interrupt Service Routines (ISRs) are defined here. They will be
;   accessed from idt.c
;   Interrupt ISRs are called, when exceptions occurr, 
;   or an 'int' asssembly instruction causes an interrupt.
;   They end up being handeled by the 'fault_handler' functio in isrs.c

; macro for ISRs which already push an error code
%macro ISR_NOERRCODE 1
    [GLOBAL isr%1]
    isr%1:
        cli
        push byte %1
        jmp  isr_common_stub
%endmacro

; macro for ISRs which do not push an error code
%macro ISR_ERRCODE 1
    [GLOBAL isr%1]
    isr%1:
        cli
        ; pushing error code 0
        push byte 0
        push byte %1
        jmp  isr_common_stub
%endmacro

; 7 and 9-14 already push error codes
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_ERRCODE   7
ISR_NOERRCODE 8
ISR_ERRCODE   9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31


; We call a C function here. We need to let the assembler know
; that 'fault_handler' exists in another file (idt.c)
[EXTERN fault_handler]

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov  ax,  0x10          ; Load the Kernel Data Segment descriptor!
    mov  ds,  ax
    mov  es,  ax
    mov  fs,  ax
    mov  gs,  ax
    mov  eax, esp           ; Push us the stack
    push eax
    mov  eax, fault_handler
    call eax                ; A special call, preserves the 'eip' register
    pop  eax
    pop  gs
    pop  fs
    pop  es
    pop  ds
    popa
    add  esp, 8             ; Cleans up the pushed error code and pushed ISR number
    iret                    ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!