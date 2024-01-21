/**
 * FILENAME :       irq.c
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
 *  Interrupt Requests are sent by hardware devices
 *  like the keyboard or system timer.
 *  Their handlers are placed in the IDT after the
 *  Interrupt service routines (ISR)
 */

#include "low_level.h"
#include "irq.h"
#include "idt.h"

/**
 * These are our custom ISRs that point to our special IRQ handler
 * instead of the regular 'fault_handler' function.
 * They will be placed behind the 32 system ISRs in the IDT
 */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/**
 * This array is actually an array of function pointers. We use
 * this to handle custom IRQ handlers for a given IRQ.
 * When an IRQ occurs, a function from this array will be called.
 * If the function is still '0', nothing will happen
 */
void *irq_routines[16] =
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0};

/**
 * Installs a custom IRQ handler for the given IRQ
 *
 * @param irq   Index of the IRQ.
 * 				IRQ 0 – system timer
 * 				IRQ 1 – keyboard on PS/2 port
 * 				IRQ 2 – cascaded signals from IRQs 8–15
 * 				IRQ 3 – serial port controller for serial port 2
 * 				IRQ 4 – serial port controller for serial port 1
 * 				IRQ 5 – parallel port 3 or sound card
 * 				IRQ 6 – floppy disk controller
 * 				IRQ 7 – parallel port 1
 * 				IRQ 8 – real-time clock (RTC)
 * 				IRQ 9 – Advanced Configuration and Power Interface (ACPI)
 * 				IRQ 10 – The Interrupt is left open for the use of peripherals
 * 				IRQ 11 – The Interrupt is left open for the use of peripherals
 * 				IRQ 12 – mouse on PS/2 port
 * 				IRQ 13 – CPU co-processor or FPU or inter-processor interrupt
 * 				IRQ 14 – primary ATA channel
 * 				IRQ 15 – secondary ATA channel
 * @param handler Function to call when IRQ fires
 *
 * @return void
 */
void irq_install_handler(int irq, void (*handler)(struct regs *regs))
{
    irq_routines[irq] = handler;
}

/**
 * Clears the handler for a given IRQ
 *
 * @param irq Index of the IRQ.
 *
 * @return void
 */
void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}

/**
 * Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
 * is a problem in protected mode, because IDT entry 8 is a
 * Double Fault! Without remapping, every time IRQ0 fires,
 * you get a Double Fault Exception, which is NOT actually
 * what's happening. We send commands to the Programmable
 * Interrupt Controller (PICs - also called the 8259's) in
 * order to make IRQ0 to 15 be remapped to IDT entries 32 to
 * 47
 *
 * @return void
 */
void irq_remap(void)
{
    // write ICW1 to PICM, we are gonna write commands to PICM
    port_byte_out(0x20, 0x11);
    // write ICW1 to PICS, we are gonna write commands to PICS
    port_byte_out(0xA0, 0x11);

    // remap PICM to 0x20 (32 decimal)
    port_byte_out(0x21, 0x20);
    // remap PICS to 0x28 (40 decimal)
    port_byte_out(0xA1, 0x28);

    // IRQ2 -> connection to slave
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);

    // write ICW4 to PICM, we are gonna write commands to PICM
    port_byte_out(0x21, 0x01);
    // write ICW4 to PICS, we are gonna write commands to PICS
    port_byte_out(0xA1, 0x01);

    // enable all IRQs on PICM
    port_byte_out(0x21, 0x0);
    // enable all IRQs on PICS
    port_byte_out(0xA1, 0x0);
}

/**
 * We first remap the interrupt controllers, and then we install
 * the appropriate ISRs to the correct entries in the IDT. This
 * is just like installing the exception handlers
 *
 * @return void
 */
void irq_install()
{
    irq_remap();

    idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E); // 0x8E = 10001 11 0
    idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
    idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
    idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
}
/**
 * Function called by IRQ.asm
 * When an IRQ is fired, it pushes all registers to the stack in an order
 * defined by the layout of the 'struct regs'.
 * Each of the IRQ-ISRs  point to this function, rather than
 * the 'fault_handler' in 'isrs.c'.
 * For each IRQ, the approiate handler function is called from
 * irq_routines[]
 * The IRQ Controllers need
 * to be told when you are done servicing them, so you need
 * to send them an "End of Interrupt" command (0x20). There
 * are two 8259 chips: The first exists at 0x20, the second
 * exists at 0xA0. If the second controller (an IRQ from 8 to
 * 15) gets an interrupt, you need to acknowledge the
 * interrupt at BOTH controllers, otherwise, you only send
 * an EOI command to the first controller. If you don't send
 * an EOI, you won't raise any more IRQs
 *
 * @param r registers pushed in assembly
 *
 * @return void
 */
void irq_handler(struct regs *regs)
{
    // blank function pointer
    void (*handler)(struct regs *regs);

    // Find out if we have a custom handler to run for this
    // IRQ, and then finally, run it
    handler = irq_routines[regs->int_no - 32];
    if (handler)
    {
        handler(regs);
    }

    // If the IDT entry that was invoked was greater than 40
    // (meaning IRQ8 - 15), then we need to send an EOI to
    // the slave controller, to reset it
    if (regs->int_no >= 40)
    {
        port_byte_out(0xA0, 0x20);
    }

    // In either case, we need to send an EOI to the master
    //  interrupt controller too
    port_byte_out(0x20, 0x20);
}