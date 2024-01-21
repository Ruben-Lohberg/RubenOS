/**
 * FILENAME :       low_level.c
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
 *  Interface for low level functions and declarations
 */

#ifndef LOW_LEVEL_C
#define LOW_LEVEL_C

/**
 * registers as they are pushed onto the stack in assembly
 * for the "fault_handler" and "irq_handler" functions
 */
struct regs
{
    unsigned int gs, fs, es, ds;                         /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; /* pushed by 'pusha' */
    unsigned int int_no, err_code;                       /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;           /* pushed by the processor automatically */
};

unsigned char port_byte_in(unsigned short port);
void port_byte_out(unsigned short port, unsigned char data);
unsigned short port_word_in(unsigned short port);
void port_word_out(unsigned short port, unsigned short data);
unsigned char *memcpy(unsigned char *dest, const unsigned char *src,
                      unsigned int count);
unsigned char *memset(unsigned char *dest, unsigned char val,
                      unsigned int count);
unsigned short *memsetw(unsigned short *dest, unsigned short val,
                        unsigned int count);

#endif