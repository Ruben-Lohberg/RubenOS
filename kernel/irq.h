/**
 * FILENAME :       irq.h
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
 *  Interface for interrupt request code
 */

#ifndef IRQ_C
#define IRQ_C

#include "low_level.h"

void irq_install_handler(int irq, void (*handler)(struct regs *regs));
void irq_uninstall_handler(int irq);
void irq_install();

#endif