/**
 * FILENAME :       idt.c
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
 *  The Interrupt Descriptor Table shows the processor, which
 *  Interrupt Service Routine (ISR) shall be called
 *  when exceptions occurr, or an 'int' asssembly instruction causes an interrupt.
 *  It also defines what code shall be executed for Interrupt Requests (IRQ) by devices
 *  like Keyboard Interrupts.
 */

#include "idt.h"
#include "screen.h"
#include "low_level.h"

// Defines a 32-bit IDT entry
struct idt_entry
{
    // The lower 16 bits of the address to jump to when this interrupt fires.
    unsigned short base_lo;
    // Kernel segment selector
    unsigned short sel;
    // Reserved bits - This must always be zero.
    unsigned char always0;
    /**
     * Flags
     * 5 Bit Const  - Always 01110
     * 2 Bit DPL    - Which Ring (0 to 3)
     * 1 Bit P      - Segment is present? (1 = Yes)
     * */
    unsigned char flags;
    // The upper 16 bits of the address to jump to.
    unsigned short base_hi;
} __attribute__((packed));

/**
 * A struct describing a pointer to an array of interrupt handlers.
 * This is in a format suitable for giving to 'lidt'.
 */
struct idt_ptr
{
    // The length of the IDT in bit
    unsigned short limit;
    // The address of the first element in our idt_entry_t array.
    unsigned int base;
} __attribute__((packed));

/**
 * Declare an IDT of 256 entries.
 * If any undefined IDT entry is hit, it normally
 * will cause an "Unhandled Interrupt" exception. Any descriptor
 * for which the 'presence' bit is cleared (0) will generate an
 * "Unhandled Interrupt" exception
 */
struct idt_entry idt[256];

// IDT pointer for the lidt assembly instruction
struct idt_ptr idtp;

// This exists in 'idt.asm', and is used to load our IDT
extern void idt_load();

/**
 * These are function prototypes for all of the exception
 * handlers: The first 32 entries in the IDT are reserved
 * by Intel, and are designed to service exceptions!
 * Their function can be seen in the array 'exception_messages' below
 */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

unsigned char *exception_messages[] =
    {
        "Division By Zero Exception",
        "Debug Exception",
        "n Maskable Interrupt Exception",
        "Breakpoint Exception",
        "Into Detected Overflow Exception",
        "Out of Bounds Exception",
        "Invalid Opcode Exception",
        "Coprocessor Exception",
        "Double Fault Exception",
        "Coprocessor Segment Overrun Exception",
        "Bad TSS Exception",
        "Segment t Present Exception",
        "Stack Fault Exception",
        "General Protection Fault Exception",
        "Page Fault Exception",
        "Unkwn Interrupt Exception",
        "Coprocessor Fault Exception",
        "Alignment Check Exception",
        "Machine Check Exception",
        "Reserved Exceptions" // isr 19-31
};

/**
 * Function to set an IDT entry
 *
 * @param num   Index in the IDT Table
 * @param base  address to jump to when this interrupt fires
 * @param sel   Kernel segment selector
 * @param flags 5 Bit Const  - Always 01110,
 *              2 Bit DPL    - Which Ring (0 to 3),
 *              1 Bit P      - Segment is present? (1 = Yes)
 *
 * @return void
 */
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel,
                  unsigned char flags)
{
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    idt[num].sel = sel;
    idt[num].always0 = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt[num].flags = flags /* | 0x60 */;
}

/**
 * Installs the IDT based on the curren values of idtp and idt[]
 *
 * @return void
 */
void idt_install()
{
    // Sets the special IDT pointer up
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;

    // Clear out the entire IDT, initializing it to zeros
    memset((char *)&idt, 0, sizeof(struct idt_entry) * 256);
    memset((char *)&idt, 0, sizeof(struct idt_entry) * 256);

    // Add any new ISRs to the IDT here using idt_set_gate
    idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(4, (unsigned)isr4, 0x08, 0x8E);
    idt_set_gate(5, (unsigned)isr5, 0x08, 0x8E);
    idt_set_gate(6, (unsigned)isr6, 0x08, 0x8E);
    idt_set_gate(7, (unsigned)isr7, 0x08, 0x8E);
    idt_set_gate(8, (unsigned)isr8, 0x08, 0x8E);
    idt_set_gate(9, (unsigned)isr9, 0x08, 0x8E);
    idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(4, (unsigned)isr4, 0x08, 0x8E);
    idt_set_gate(5, (unsigned)isr5, 0x08, 0x8E);
    idt_set_gate(6, (unsigned)isr6, 0x08, 0x8E);
    idt_set_gate(7, (unsigned)isr7, 0x08, 0x8E);
    idt_set_gate(8, (unsigned)isr8, 0x08, 0x8E);
    idt_set_gate(9, (unsigned)isr9, 0x08, 0x8E);
    idt_set_gate(10, (unsigned)isr10, 0x08, 0x8E);
    idt_set_gate(11, (unsigned)isr11, 0x08, 0x8E);
    idt_set_gate(12, (unsigned)isr12, 0x08, 0x8E);
    idt_set_gate(13, (unsigned)isr13, 0x08, 0x8E);
    idt_set_gate(14, (unsigned)isr14, 0x08, 0x8E);
    idt_set_gate(15, (unsigned)isr15, 0x08, 0x8E);
    idt_set_gate(16, (unsigned)isr16, 0x08, 0x8E);
    idt_set_gate(17, (unsigned)isr17, 0x08, 0x8E);
    idt_set_gate(18, (unsigned)isr18, 0x08, 0x8E);
    idt_set_gate(19, (unsigned)isr19, 0x08, 0x8E);
    idt_set_gate(20, (unsigned)isr20, 0x08, 0x8E);
    idt_set_gate(21, (unsigned)isr21, 0x08, 0x8E);
    idt_set_gate(22, (unsigned)isr22, 0x08, 0x8E);
    idt_set_gate(23, (unsigned)isr23, 0x08, 0x8E);
    idt_set_gate(24, (unsigned)isr24, 0x08, 0x8E);
    idt_set_gate(25, (unsigned)isr25, 0x08, 0x8E);
    idt_set_gate(26, (unsigned)isr26, 0x08, 0x8E);
    idt_set_gate(27, (unsigned)isr27, 0x08, 0x8E);
    idt_set_gate(28, (unsigned)isr28, 0x08, 0x8E);
    idt_set_gate(29, (unsigned)isr29, 0x08, 0x8E);
    idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
    idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);

    /* Points the processor's internal register to the new IDT */
    idt_load();
}

/**
 * Function called by ISR.asm
 * When an ISR is fired, it pushes all registers to the stack in an order
 * defined by the layout of the 'struct regs'.
 * The fault_handler function then prints an exception message if it's
 * a system exception (IDT 0-31)
 *
 * @param r registers pushed in assembly
 *
 * @return void
 */
void fault_handler(struct regs *regs)
{
    // Is this a fault whose number is from 0 to 31?
    if (regs->int_no < 32)
    {
        // Display the description for the Exception that occurred.
        // Then we will simply halt the system using an infinite loop for now
        print(exception_messages[regs->int_no], DEFAULT_COLOR_SCHEME);
        print(" Exception. System Halted!\n", DEFAULT_COLOR_SCHEME);
        for (;;)
            ;
        for (;;)
            ;
    }
}