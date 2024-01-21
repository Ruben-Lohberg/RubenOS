/**
 * FILENAME :       screen.c
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
 *  Screen driver using direct memory access to print chars to the screen
 */

#include "screen.h"
#include "low_level.h"
#include "string.h"

// beginning of memory mapped IO video address
#define VIDEO_ADRESS 0xb8000
// number of rows on screen
#define MAX_ROWS 25
// number of columns on screen
#define MAX_COLUMNS 80

// Screen device IO ports
// Control port
#define REG_SCREEN_CONTROL 0x3D4
// Data port
#define REG_SCREEN_DATA 0x3D5

// internal function declarations

void set_cursor(int column, int row);
int handle_scrolling(int cursor_offset);
int get_cursor();
int get_screen_position(int column, int row);
int get_row(int offset);
int get_column(int offset);
void scroll_up(int rows);
void set_cursor(int column, int row);

/**
 * Prints a char on screen by inserting it into memory along the attribute
 * byte at the specified location. Will not move the cursor or scroll
 * the screen.
 * If the row index is below 0 or above the maximum, it it will over-/underflow
 * to another column.
 * If the column index is below 0, the char will be printed at 0,0.
 * If the column index is above the maximum, the char will be printed in the
 * last row, but at the specified column.
 *
 * @param character ASCII character to print
 * @param column    Column or x coordinate to print at
 * @param row       Row or y coordinate to print at
 * @param attribute_byte    Byte setting attributes like color. If it is 0, the
 *                          default style will be used
 *
 * @return void
 */
void print_char_at(char character, int column, int row,
                   unsigned char attribute_byte)
{
    // pointer to start of video memory
    unsigned char *vidmem = (unsigned char *)VIDEO_ADRESS;

    // offset from video memory start for the defined row & column
    int offset = get_screen_position(column, row) * 2;

    // writing the charactet if it's not a newline
    vidmem[offset] = character;
    vidmem[offset + 1] = attribute_byte;
}

/**
 * Prints a char at the current cursor position. Will advance the cursor and
 * will scroll if the cursor tries to advance past the last row.
 *
 * @param character ASCII character to print
 * @param attribute_byte    Byte setting attributes like color
 *
 * @return void
 */
void print_char(char character, unsigned char attribute_byte)
{
    int current_cursor = get_cursor();
    int row = get_row(current_cursor);

    // if the character is a newline
    if (character == '\n')
    {
        // advance cursor to the beginning of the next line. Might scroll
        set_cursor(0, row + 1);
    }
    else
    {
        int column = get_column(current_cursor);
        // print at cursor
        print_char_at(character, column, row, attribute_byte);
        // advance cursor once
        move_cursor(1, 0);
    }
}

/**
 * Prints string at specified location. Will not move the cursor or scroll
 * the screen.
 * If the row index is below 0 or above the maximum, it it will over-/underflow
 * to another column.
 * If the column index is below 0, the char will be printed at 0,0.
 * If the column index is above the maximum, the char will be printed in the
 * last row, but at the specified column.
 *
 * @param message 0 terminated string
 * @param column x-axis
 * @param row y-axis
 *
 * @return void
 */
void print_at(char *message, int column, int row, unsigned char attribute_byte)
{
    int i = 0;
    while (message[i] != 0)
    {
        if (row * MAX_COLUMNS + column < MAX_ROWS * MAX_COLUMNS)
        {
            print_char_at(message[i], column++, row, attribute_byte);
        }
        i++;
    }
}

/**
 * Prints a message at the current cursor position. Will advance the cursor and
 * will scroll if the cursor tries to advance past the last row.
 *
 * @param message 0 terminated string
 *
 * @return void
 */
void print(char *message, unsigned char attribute_byte)
{
    int i = 0;
    while (message[i] != 0)
    {
        print_char(message[i], attribute_byte);
        i++;
    }
}

/**
 * Calulates a 1-dimensional screen position. Will return a valid position
 *
 * @param row y-axis
 * @param column x-axis
 *
 * @retuns 1-dimensional position. Not memory offset! Can be multiplied by
 * 2 to get memory offset
 */
int get_screen_position(int column, int row)
{
    // if the column is below 0, it will underflow into a lower row
    if (column < 0)
    {
        row -= ((-column) / MAX_COLUMNS) + 1;
        column = MAX_COLUMNS - (-column % MAX_COLUMNS);
    }
    // if the row is below 0, we will print at 0,0
    if (row < 0)
    {
        row = column = 0;
    }
    // if column is above the maximum, it will overflow into a higher row
    if (column >= MAX_COLUMNS)
    {
        row += column / MAX_COLUMNS;
        column %= MAX_COLUMNS;
    }
    // if row is above maximum, we will print at the last row
    if (row >= MAX_ROWS)
    {
        row = MAX_ROWS - 1;
    }
    // row * width of a row + column.
    // *2 because each character cell is two bytes wide,
    // one byte for the char, one byte for the attribute
    return (row * MAX_COLUMNS + column);
}

/**
 * Calculates the row from a screen memory offset. Can return row numbers
 * outside of video memory.
 *
 * @param offset Offset into video memory from a char or the cursor
 *
 * @return row
 */
int get_row(int offset)
{
    return offset / MAX_COLUMNS;
}

/**
 * Calculates the column from a screen memory offset.
 *
 * @param position position into video memory from a char or the cursor
 *
 * @return column between 0 and MAX_COLUMNS-1
 */
int get_column(int position)
{
    if (position < 0)
    {
        int max_position = MAX_COLUMNS * MAX_ROWS;
        position = -position % max_position;
    }
    return position % MAX_COLUMNS;
}

/**
 * Gets the current position of the cursor as a 1 dimensional value
 *
 * @returns 1-dimensional position. Can be multiplied by 2 to get memory
 * offset
 */
int get_cursor()
{
    // the screen device uses its control registers as an index to
    // selelect internal registers
    // reg 14 is the high byte of the cursor's offset
    // reg 15 is the low byte
    // select the internal register and read from it :

    // select high byte
    port_byte_out(REG_SCREEN_CONTROL, 14);
    // read high byte and shift it over 8 bit, because it is the high byte...
    int position = port_byte_in(REG_SCREEN_DATA) << 8;
    // select low byte
    port_byte_out(REG_SCREEN_CONTROL, 15);
    position += port_byte_in(REG_SCREEN_DATA);
    // The device reports the cursor position as the number of characters.
    return position;
}

/**
 * Sets the cursor to specified position. Position can be calculated using
 *
 * @param column x-axis
 * @param row y-axis
 *
 * @return void
 */
void set_cursor(int column, int row)
{
    // if the column is below 0, it will underflow into a lower row
    if (column < 0)
    {
        row -= ((-column) / MAX_COLUMNS) + 1;
        column = MAX_COLUMNS - (-column % MAX_COLUMNS);
    }

    // if column is above the maximum, it will overflow into a higher row
    if (column >= MAX_COLUMNS)
    {
        row += column / MAX_COLUMNS;
        column %= MAX_COLUMNS;
    }

    // scrolling back down
    if (row < 0)
    {
        // scroll_down(-row); needs scrolling history
        row = 0;
    }

    // scrolling up
    if (row >= MAX_ROWS)
    {
        if (row > 2 * MAX_ROWS)
        {
            row = 2 * MAX_ROWS;
        }
        scroll_up(row - MAX_ROWS + 1);
        row = MAX_ROWS - 1;
    }

    int position = get_screen_position(column, row);
    // convert from character cells in memory to characters on the screen
    int position_high_byte = position >> 8;
    int position_low_byte = position - (position_high_byte << 8);
    // similar to get_cursor() but now we write to the data registers
    // select high byte
    port_byte_out(REG_SCREEN_CONTROL, 14);
    // write high byte
    port_byte_out(REG_SCREEN_DATA, position_high_byte);
    // select low byte
    port_byte_out(REG_SCREEN_CONTROL, 15);
    // write low byte
    port_byte_out(REG_SCREEN_DATA, position_low_byte);
}

/**
 * Moves the cursor from the current position.
 *
 * @param column_offset How far the cursor shall be moved on the x axis. Takes
 * positive or negative values
 * @param row_offset How far the cursor shall be moved on the y axis. Takes
 * positive or negative values
 *
 * @return void
 */
void move_cursor(int column_offset, int row_offset)
{
    int current_cursor = get_cursor();
    int row = get_row(current_cursor);
    row += row_offset;
    int column = get_column(current_cursor);
    column += column_offset;
    set_cursor(column, row);
}

/**
 * Scrolls the screen up, when the cursor leves it to the lower end.
 *
 * @param rows number of rows to scroll up
 *
 * @return void
 */
void scroll_up(int rows)
{
    for (int j = 0; j < rows; j++)
    {
        /**
         * Shuffle all the rows one row back (except for the first row, we dont want
         * to write its contents into a memory region outside of video memory)
         */
        for (int i = 1; i < MAX_ROWS; i++)
        {
            memcpy((unsigned char *)(get_screen_position(0, i - 1) * 2 + VIDEO_ADRESS), // the row above it
                   (unsigned char *)(get_screen_position(0, i) * 2 + VIDEO_ADRESS),     // a row
                   MAX_COLUMNS * 2);                                                    // length of a row in bytes
        }
        // Blank the last line
        char *last_line = (char *)(get_screen_position(0,
                                                       MAX_ROWS - 1) *
                                   2) +
                          VIDEO_ADRESS;
        for (int i = 0; i < MAX_COLUMNS * 2 - 1; i++)
        {
            last_line[i] = 0;
        }
    }
}

/**
 * Clears the screen, by setting all of video memory to 0
 *
 * @return void
 */
void clear_screen()
{
    memset((unsigned char *)VIDEO_ADRESS, 0, 2 * MAX_ROWS * MAX_COLUMNS);
    set_cursor(0, 0);
}

/**
 * Calculates the length of an unsigned int
 *
 * @param input unsigned int to get the length of
 *
 * @return int length, number of digits
 */
int unsigned_int_length(unsigned int input)
{
    if (input == 0)
    {
        return 1;
    }
    int count = 0;
    while (input != 0)
    {
        count++;
        input /= 10;
    }
    return count;
}

/**
 * Prints an unsigned int at the cursor
 *
 * @param input unsigned int to print
 * @param attribute_byte Attribute byte to use when printing
 *
 * @return void
 */
void print_unsigned_int(unsigned int input, unsigned char attribute_byte)
{
    int string_length = unsigned_int_length(input) + 1;
    char result[string_length];
    result[string_length - 1] = 0;
    for (int i = string_length - 2; i >= 0; i--)
    {
        result[i] = input % 10 + 48;
        input /= 10;
    }
    print(result, attribute_byte);
}

/**
 * Prints a signed int at the cursor
 *
 * @param input int to print
 * @param attribute_byte Attribute byte to use when printing
 *
 * @return void
 */
void print_int(int input, unsigned char attribute_byte)
{
    if (input < 0)
    {
        print_char('-', attribute_byte);
        input = -input;
    }
    print_unsigned_int(input, attribute_byte);
}

/**
 * Prints an unsigned int at the specified position. Will print a string
 * of a fixed length. To match that length, leading zeroes will be inserted.
 *
 * @param input unsigned int to print
 * @param length length of the printed string
 * @param column x-coordinate to print at
 * @param row y-coordinate to print at
 */
void print_unsigned_int_leading_zeroes_at(unsigned int input,
                                          unsigned int length, int column,
                                          int row, unsigned char attribute_byte)
{
    char result[length + 1];
    result[length] = 0;
    for (int i = length - 1; i >= 0; i--)
    {
        result[i] = input % 10 + 48;
        input /= 10;
    }
    print_at(result, column, row, attribute_byte);
}

/**
 * Prints the time string to the top left of the screen
 *
 * @param time Time string 00:00:00
 */
void print_time(unsigned int seconds)
{
    unsigned int minutes = seconds / 60;
    seconds %= 60;
    unsigned int hours = minutes / 60;
    minutes %= 60;

    print_unsigned_int_leading_zeroes_at(seconds, 2, MAX_COLUMNS - 2, 0,
                                         DEFAULT_COLOR_SCHEME);
    print_char_at(':', MAX_COLUMNS - 3, 0, DEFAULT_COLOR_SCHEME);
    print_unsigned_int_leading_zeroes_at(minutes, 2, MAX_COLUMNS - 5, 0,
                                         DEFAULT_COLOR_SCHEME);
    print_char_at(':', MAX_COLUMNS - 6, 0, DEFAULT_COLOR_SCHEME);
    print_unsigned_int_leading_zeroes_at(hours, 2, MAX_COLUMNS - 8, 0,
                                         DEFAULT_COLOR_SCHEME);
}