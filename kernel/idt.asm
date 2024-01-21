;  FILENAME :    	idt.asm
;
;  AUTHOR :      	Ruben Lohberg 
;
;  START DATE:   	19 Nov 2023
;
;  LAST UPDATE: 	21 Jan 2024
;
;  PROJECT:   	  RubenOS
;
;  DESCRIPTION:
;   Loads the Interrupt Desciptor Table defined in 'idtp' into the processor. 
;   The IDT shows the processor, which Interrupt Service Routine (ISR) shall be called
;   when exceptions occurr, or an 'int' asssembly instruction causes an interrupt.
;   It also defines what code shall be executed for Interrupt Requests (IRQ) by devices
;   like Keyboard Interrupts.
;   idtp is found in idt.c as 'struct idt_ptr idtp'
;   This function is declared in idt.c as 'extern void idt_load();'
[GLOBAL idt_load]
[EXTERN idtp]
idt_load:
    lidt [idtp]
    ret