/**
 * FILENAME :       screen.h
 *
 * AUTHOR :         Ruben Lohberg
 *
 * START DATE :     25 Oct 2023
 *
 * LAST UPDATE :    07 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  Interface for screen driver
 */

#ifndef SCREEN_H
#define SCREEN_H

#define WHITE_ON_BLACK 0x0f
// Attribute for our default color scheme
#define DEFAULT_COLOR_SCHEME WHITE_ON_BLACK

void print_char_at(char character, int column, int row,
                   unsigned char attribute_byte);
void print_char(char character, unsigned char attribute_byte);
void print_at(char *message, int row, int col, unsigned char attribute_byte);
void print(char *message, unsigned char attribute_byte);
void clear_screen();
void print_unsigned_int(unsigned int input, unsigned char attribute_byte);
void print_int(int input, unsigned char attribute_byte);
void move_cursor(int column_offset, int row_offset);
void print_time(unsigned int seconds);

#endif