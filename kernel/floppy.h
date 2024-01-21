/**
 * FILENAME :       floppy.h
 *
 * AUTHOR :         Teemu Voipio / Ruben Lohberg
 *
 * START DATE :     8 Mar 2007
 *
 * LAST UPDATE :    07 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  Constants for the floppy driver.
 */

#ifndef FLOPPY_C
#define FLOPPY_C

/**
 * statically reserve memory for DMA transfer
 */
#define FLOPPY_DMA_LENGTH 0x4000
extern char floppy_dmabuf[FLOPPY_DMA_LENGTH];

void floppy_install();
void floppy_write_buffer(unsigned int index);
void floppy_read_buffer(unsigned int index);
void floppy_clear_buffer();

void floppy_reset_and_calibrate();

#endif
