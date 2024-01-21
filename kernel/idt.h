/**
 * FILENAME :       idt.h
 *
 * AUTHOR :         Ruben Lohberg
 *
 * START DATE :     19 Nov 2023
 *
 * LAST UPDATE :    21 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  Interface for Interrupt Descriptor Table code
 */

#ifndef IDT_C
#define IDT_C

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel,
                  unsigned char flags);
void idt_install();

#endif