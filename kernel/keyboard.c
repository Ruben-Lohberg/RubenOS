/**
 * FILENAME :       keyboard.c
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
 *  Keyboard driver. Using Interrupts to read scan codes from the keyboard.
 */

#include "timer.h"
#include "screen.h"
#include "low_level.h"
#include "bool.h"
#include "irq.h"

/**
 * German qwertz keyboard.
 * [0] is regular
 * [1] is shift/caps
 * [2] is alt gr
 */
unsigned char keyboard_de[3][128] =
    {
        {0,  // the is no 0 key
         27, // 0x01 esc
         '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 's', '`',
         '\b', // 0x0e backspace
         '\t', // 0x0f tab
         'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 'u', '+',
         '\n', // 0x1c Enter key
         0,    // 0x1d Control
         'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'o', 'a', '^',
         0, // 0x2a Left shift
         '#', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-',
         0, // 0x36 Right shift
         '*',
         0,   // 0x38 Alt
         ' ', // 0x39 Space bar
         0,   // 0x3a Caps lock

         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x3b-0x44 F1-F10 key

         0, // 0x45 Num lock
         0, // 0x46 Scroll Lock
         0, // 0x47 Home key
         0, // 0x48 Up Arrow
         0, // 0x49 Page Up
         '-',
         0, // 0x4b Left Arrow
         0, // 0x4c Left Arrow
         0, // 0x4d Right Arrow
         '+',
         0,   // 0x4f End key*/
         0,   // 0x50 Down Arrow
         0,   // 0x51 Page Down
         0,   // 0x52 Insert Key
         0,   // 0x53 Delete Key
         0,   // 0x54 Print screen
         0,   // 0x55 doesn't exist?
         '<', // 0x56
         0,   // 0x57 F11 Key
         0,   // 0x58 F12 Key
         0,   // 0x59 doesn't exist?
         /* All other keys are undefined */},
        {0,  // the is no 0 key
         27, // 0x01 esc
         '!', '"', '3', '$', '%', '&', '/', '(', ')', '=', '?', '`',
         '\b', // 0x0e backspace
         '\t', // 0x0f tab
         'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 'U', '*',
         '\n', // 0x1c Enter key
         0,    // 0x1d Control
         'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'O', 'A', '^',
         0, // 0x2a Left shift
         '\'', 'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_',
         0, // 0x36 Right shift
         '*',
         0,   // 0x38 Alt
         ' ', // 0x39 Space bar
         0,   // 0x3a Caps lock

         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x3b-0x44 F1-F10 key

         0, // 0x45 Num lock
         0, // 0x46 Scroll Lock
         0, // 0x47 Home key
         0, // 0x48 Up Arrow
         0, // 0x49 Page Up
         '-',
         0, // 0x4b Left Arrow
         0, // 0x4c Left Arrow
         0, // 0x4d Right Arrow
         '+',
         0,   // 0x4f End key*/
         0,   // 0x50 Down Arrow
         0,   // 0x51 Page Down
         0,   // 0x52 Insert Key
         0,   // 0x53 Delete Key
         0,   // 0x54 Print screen
         0,   // 0x55 doesn't exist?
         '>', // 0x56
         0,   // 0x57 F11 Key
         0,   // 0x58 F12 Key
         0,   // 0x59 doesn't exist?
         /* All other keys are undefined */},
        {0,  // the is no 0 key
         27, // 0x01 esc
         '1', '2', '3', '4', '5', '6', '{', '[', ']', '}', '\\', '`',
         '\b', // 0x0e backspace
         '\t', // 0x0f tab
         '@', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 'u', '~',
         '\n', // 0x1c Enter key
         0,    // 0x1d Control
         'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'o', 'a', '^',
         0, // 0x2a Left shift
         '#', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-',
         0, // 0x36 Right shift
         '*',
         0,   // 0x38 Alt
         ' ', // 0x39 Space bar
         0,   // 0x3a Caps lock

         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x3b-0x44 F1-F10 key

         0, // 0x45 Num lock
         0, // 0x46 Scroll Lock
         0, // 0x47 Home key
         0, // 0x48 Up Arrow
         0, // 0x49 Page Up
         '-',
         0, // 0x4b Left Arrow
         0, // 0x4c Left Arrow
         0, // 0x4d Right Arrow
         '+',
         0,   // 0x4f End key*/
         0,   // 0x50 Down Arrow
         0,   // 0x51 Page Down
         0,   // 0x52 Insert Key
         0,   // 0x53 Delete Key
         0,   // 0x54 Print screen
         0,   // 0x55 doesn't exist?
         '|', // 0x56
         0,   // 0x57 F11 Key
         0,   // 0x58 F12 Key
         0,   // 0x59 doesn't exist?
         /* All other keys are undefined */}};

/**
 * pointer to the currently chosen keyboard
 */
unsigned char (*current_keyboard)[3][128] = &keyboard_de;

/**
 * Setting for the keyboard mode
 * [0] is regular
 * [1] is shift/caps
 * [2] is alt gr
 */
unsigned char keyboard_mode = 0;

/**
 * Function which will be called to print a key.
 */
void (*print_function)(char key);

/**
 * default print function, which just prints the char to the cursor
 *
 * @param key key to print
 */
static void default_print_function(char key)
{
    print_char(key, DEFAULT_COLOR_SCHEME);
}

/**
 * Sets the print_function to a custom function.
 *
 * @param function custom print function taking a char and returning void
 */
void keyboard_set_print_function(void (*function)(char))
{
    print_function = function;
}

/**
 * Sets the print_function to default_print_function
 */
void keyboard_set_default_print_function()
{
    keyboard_set_print_function(default_print_function);
}

/**
 * The function run by the Interrupt handler when there is a keyboard
 * interrupt
 *
 * @param regs CPU registers as specified in low_level.h
 */
static void keyboard_callback(struct regs *regs)
{
    // getting rid of surpressed parameter warnings
    (void)(regs);
    /* Read from the keyboard's data buffer */
    unsigned char scancode = port_byte_in(0x60);

    /* If the top bit of the byte we read from the keyboard is
     *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        scancode &= 0x7f; // getting rid of the leading 1 bit
        /* You can use this one to see if the user released the
         *  shift, alt, or control keys... */
        switch (scancode)
        {
        case 0x2a: // left shift
        case 0x36: // right shift
            keyboard_mode = 0;
            break;
        case 0x38: // alt
            keyboard_mode = 0;
            break;
        }
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
         *  hold a key down, you will get repeated key press
         *  interrupts. */
        switch (scancode)
        {
        case 0x38: // alt
            keyboard_mode = 2;
            break;
        case 0x2a: // left shift
        case 0x36: // right shift
            keyboard_mode = 1;
            break;
        case 0x3a: // caps lock
            if (keyboard_mode != 2)
            {
                keyboard_mode = !keyboard_mode;
            }
            break;
        default:
            char key = (*current_keyboard)[keyboard_mode][scancode];
            if (key != 0)
            {
                print_function(key);
            }
            break;
        }
    }
}

/**
 * Install the keyboard driver
 */
void keyboard_install()
{
    irq_install_handler(1, &keyboard_callback);
    keyboard_set_default_print_function();
}