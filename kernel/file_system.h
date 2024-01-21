/**
 * FILENAME :       file_system.c
 *
 * AUTHOR :         Ruben Lohberg
 *
 * START DATE :     16 Dec 2023
 *
 * LAST UPDATE :    07 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  Structures and constants for the file system.
 */

#ifndef FILE_SYSTEM_C
#define FILE_SYSTEM_C

#include "floppy.h"

#define MAX_FILENAME_LENGTH 60

/**
 * A primitive file format.
 */
typedef struct file
{
    // Filename. Zero-terminated string with maximum 59 chars.
    char name[MAX_FILENAME_LENGTH];
    // Length of the data in bytes
    unsigned int data_length;
    // Data consisting of maximum 18367 bytes
    unsigned char *data[FLOPPY_DMA_LENGTH - MAX_FILENAME_LENGTH - 33];
    // a zero bit at the end to ensure that printing the file doesn't overflow
    unsigned zero : 1;
} __attribute__((packed)) file;

void install_filesystem();

#endif