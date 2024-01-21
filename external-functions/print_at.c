/**
 * FILENAME :       draw_logo.c
 *
 * AUTHOR :         Ruben Lohberg
 *
 * START DATE :     08 Jan 2024
 *
 * LAST UPDATE :    08 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  An external function which will be loaded onto the data-floppy before
 *  (or while) the Operating system is running.
 *  Prints a RubenOS logo to the screen.
 */

// beginning of memory mapped IO video address
#define VIDEO_ADRESS 0xb8000
// number of rows on screen
#define MAX_ROWS 25
// number of columns on screen
#define MAX_COLUMNS 80

#define RED_ON_BLACK 0x4

void print_char_at(char character, int column, int row,
                   unsigned char attribute_byte);
void print_at(char *message, int column, int row, unsigned char attribute_byte);
int get_screen_position(int column, int row);
unsigned int string_to_unsigned_int(char *string);

int function_main(int argc, char **argv)
{
    print_at(argv[1],
             string_to_unsigned_int(argv[2]),
             string_to_unsigned_int(argv[3]),
             RED_ON_BLACK);
    return 0;
}

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

unsigned int string_to_unsigned_int(char *string)
{
    int i = 0;
    unsigned int result = 0;
    while (string[i] != 0)
    {
        result *= 10;
        // converts char '0' -> 0, '1' -> 1 ...
        string[i] -= 48;
        // if a char does not represent a digit
        if (string[i] < 0 || string[i] > 9)
        {
            return 0;
        }
        result += string[i];
        i++;
    }
    return result;
}