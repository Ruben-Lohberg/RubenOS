/**
 * FILENAME :       low_level.c
 *
 * AUTHOR :         Ruben Lohberg
 *
 * START DATE :     19 Nov 2023
 *
 * LAST UPDATE :    07 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  Collection of low level functions for memory manipulation an port
 *  communication.
 */

#include "low_level.h"

/**
 * Reads a byte from a port.
 *
 * @param port port to read from
 * @return unsigned char byte value read from port
 */
unsigned char port_byte_in(unsigned short port)
{
    // reads a byte from the specified port
    // unsigned char result;
    // __asm__("in %%dx, %%al" // input from port dx into al
    //         : "=a"(result)  // put content of a register into result when finished
    //         : "d"(port));   // load value of port into d register beforehand
    // return result;
    unsigned char ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

/**
 * Writes a byte to a port.
 *
 * @param port port to write to
 * @param data byte to write
 */
void port_byte_out(unsigned short port, unsigned char data)
{
    // writes a byte to the specified port
    // __asm__("out %%al, %%dx" // output from al into port dx
    //         :
    //         : "a"(data), "d"(port));
    // load register a with data and register d with port
    asm volatile("outb %1, %0" : : "dN"(port), "a"(data));
}

/**
 * Reads a word from a port.
 *
 * @param port port to read from
 * @return unsigned short word value read from port
 */
unsigned short port_word_in(unsigned short port)
{
    unsigned short result;
    __asm__("in %%dx, %%ax"
            : "=a"(result)
            : "d"(port));
    return result;
}

/**
 * writes a word to a port.
 *
 * @param port port to write to
 * @param data word to write
 */
void port_word_out(unsigned short port, unsigned short data)
{
    __asm__("out %%ax, %%dx"
            :
            : "a"(data), "d"(port));
}

/**
 * Copy 'count' bytes of data from 'source' to 'destination'. Return 'destination'
 *
 * @param destination Memory location to write to
 * @param source Memory location to read from
 * @param count Length of copied segment
 * @return unsigned char* destination
 */
unsigned char *memcpy(unsigned char *destination, const unsigned char *source, unsigned int count)
{
    while (count != 0)
    {
        *destination++ = *source++;
        count--;
    }
    return destination;
}

/**
 * Just memcopy, but it might be needed by the compiler
 *
 * @param destination Memory location to write to
 * @param source Memory location to read from
 * @param count Length of copied segment
 * @return unsigned char* destination
 */
unsigned char *memmove(unsigned char *destination, const unsigned char *source, unsigned int count)
{
    return memcpy(destination, source, count);
}

/**
 * Memory compare. Compares two secions of memory
 *
 * @param ptr1 start address of secion 1
 * @param ptr2 start address of section 2
 * @param count length of sections to compare
 *
 * @return 0 if the sections are equeal. Positive or negative value if the
 * first encountered different byte of ptr1 is smaller or bigger
 */
int memcmp(unsigned char *ptr1, const unsigned char *ptr2, unsigned int count)
{
    for (unsigned int i = 0; i < count; i++)
    {
        int diff = ptr1[i] - ptr2[i];
        if (diff != 0)
        {
            return diff;
        }
    }
    return 0;
}

/**
 * set 'count' bytes to 'value' at 'destination'. Return 'destination'
 *
 * @param destination Memory location to write to
 * @param value byte value to set the memory to
 * @param count length of set segment in bytes
 * @return unsigned char* destination
 */
unsigned char *memset(unsigned char *destination, unsigned char value, unsigned int count)
{
    for (unsigned int i = 0; i < count; i++)
    {
        destination[i] = value;
    }
    return destination;
}

/**
 * set 'count' words to 'value' at 'destination'. Return 'destination'
 *
 * @param destination Memory location to write to
 * @param value word value to set the memory to
 * @param count length of set segment in words
 * @return unsigned short* destination
 */
unsigned short *memsetw(unsigned short *destination, unsigned short value, unsigned int count)
{
    for (unsigned int i = 0; i < count; i++)
    {
        destination[i] = value;
    }
    return destination;
}
