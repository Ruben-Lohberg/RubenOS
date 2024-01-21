/**
 * FILENAME :       shell.c
 *
 * AUTHOR :         Ruben Lohberg
 *
 * START DATE :     3 Dec 2023
 *
 * LAST UPDATE :    07 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  A primitive shell to allow the user to run commands.
 */

#include "screen.h"
#include "keyboard.h"
#include "low_level.h"
#include "string.h"

// maximum length of a user input string
#define COMMAND_BUFFER_SIZE 1024
// maximum amount of registered commands
#define MAX_COMMAND_COUNT 64
// maximum length of command name string
#define MAX_COMMAND_NAME_LENGTH 32

// color schemes
#define INPUT_COLOR_SCHEME 0x2
#define ERROR_COLOR_SCHEME 0x4

/**
 * Buffer in which a command will be written until a \n is encountered and
 * the command will be ran.
 */
char command_buffer[COMMAND_BUFFER_SIZE];

/**
 * pointing to the next free index in the command buffer
 */
int command_buffer_pointer = 0;

/**
 * pointing to the next free index in the command table
 */
int command_table_index = 0;

/**
 * 0-terminated strings, names of the commands
 */
char command_table_names[MAX_COMMAND_COUNT][MAX_COMMAND_NAME_LENGTH];

/**
 * function pointers, for commands to execute
 */
void *command_table_functions[MAX_COMMAND_COUNT];

/**
 * Prints the command prompt
 */
void print_prompt()
{
    print("$ ", DEFAULT_COLOR_SCHEME);
}

/**
 * Registers a new command to be known under a name from now on
 *
 * @param name null terminated string
 * @param function function pointer
 */
void register_command(char *name, int (*function)(int argc, char **argv))
{
    string_copy(name, command_table_names[command_table_index]);
    command_table_functions[command_table_index] = function;
    command_table_index++;
}

/**
 * Clears the command buffer and resets the pointer
 */
void clear_command_buffer()
{
    memset((unsigned char *)command_buffer, 0, COMMAND_BUFFER_SIZE);
    command_buffer_pointer = 0;
}

/**
 * Default function to run, if a command was not found
 *
 * @param command
 */
int default_function(int argc, char **argv)
{
    // avoid unised parameter warning
    (void)(argc);
    print("Could not find command: '", ERROR_COLOR_SCHEME);
    print(argv[0], DEFAULT_COLOR_SCHEME);
    print("'\n", ERROR_COLOR_SCHEME);
    return 0;
}

/**
 * execute a command
 *
 * @param command 0 terminated string. Command name
 * @param args program arguments. Contains the command itself
 */
void execute_command(char *command, char *args)
{
    // reduce consecutive strings of whitespaces to a single ' '
    reduce_consecutive_occurrences(args, ' ');
    // arguments words split into zero terminated strings. Array has space
    // for pointers to all arguments + a null pointer at the end
    char *argv[string_count_char(args, ' ') + 1];
    // first argument is the command name
    argv[0] = command;
    // amount of arguments
    unsigned int argc = 1;
    // iterating over args string
    unsigned int i = 0;
    while (args[i] != 0)
    {
        if (args[i] == ' ')
        {
            // when the end of a word is encountered, the delimiting whitespace is
            // exchanged for a 0 bit to make zero terminated strings out of the
            // arguments.
            args[i] = 0;
            // the next argument starts after the whitepace / zero byte
            argv[argc] = args + i + 1;
            argc++;
        }
        i++;
    }
    // if there was a trailing whitespace in the command arguments, the loop
    // above assumed, there was another argument after.
    if (argv[argc - 1] == 0)
    {
        argc--;
    }
    argv[argc] = 0;

    // in case, we don't find a function with the given command name, we execute
    // the default fruction instead
    int (*function)(int argc, char **argv) = default_function;
    // catching empty command names
    if (command[0] != 0)
    {
        // iterating over knowsn commands
        for (unsigned int i = 0; i < MAX_COMMAND_COUNT; i++)
        {
            if (string_equals(command, command_table_names[i]))
            {
                // replacing the default function with the found function
                function = command_table_functions[i];
            }
        }
    }
    function(argc, argv);
}

/**
 * keyboard print function which shall be executed by the keyboar driver
 * whenever a key is supposed to be printed.
 *
 * @param key char to be printed
 */
void shell_keyboard_print_function(char key)
{
    switch (key)
    {
    case '\n':
        // newline
        print_char(key, 0);
        if (command_buffer_pointer > 0)
        {
            char command[MAX_COMMAND_NAME_LENGTH];
            memset((unsigned char *)command, 0, MAX_COMMAND_NAME_LENGTH);
            int i = 0;
            while (command_buffer[i] != ' ' && command_buffer[i] != 0)
            {
                command[i] = command_buffer[i];
                i++;
            }
            command[MAX_COMMAND_NAME_LENGTH - 1] = 0;
            execute_command(command, command_buffer);
        }

        clear_command_buffer();
        print_prompt();
        break;
    case '\b':
        // backspace
        if (command_buffer_pointer > 0)
        {
            move_cursor(-1, 0);
            print_char(0, 0);
            move_cursor(-1, 0);
            command_buffer_pointer--;
            command_buffer[command_buffer_pointer] = 0;
        }

        break;
    default:
        command_buffer[command_buffer_pointer] = key;
        command_buffer_pointer++;
        print_char(key, INPUT_COLOR_SCHEME);
        break;
    }
}

/**
 * Simple help program listing all currently registered commands
 *
 * @param args
 */
static int help_command(int argc, char **argv)
{
    // getting rid of surpressed parameter warnings
    (void)(argc);
    (void)(argv);
    print("List of all available commands:\n", DEFAULT_COLOR_SCHEME);
    for (int i = 0; i < command_table_index; i++)
    {
        print(" ", DEFAULT_COLOR_SCHEME);
        print(command_table_names[i], DEFAULT_COLOR_SCHEME);
        print("\n", DEFAULT_COLOR_SCHEME);
    }
    return 0;
}

/**
 * Gets the ball rolling allowing user input
 *
 */
void start_shell()
{
    print_char('\n', 0);
    print("Starting the shell...\n", DEFAULT_COLOR_SCHEME);
    clear_command_buffer();
    memset((void *)command_table_names, 0, COMMAND_BUFFER_SIZE);
    for (int i = 0; i < MAX_COMMAND_COUNT; i++)
    {
        command_table_functions[i] = 0;
    }
    register_command("help", help_command);
    print("Use command 'help' for a list of all available commands\n",
          DEFAULT_COLOR_SCHEME);
    print_prompt();
    keyboard_set_print_function(shell_keyboard_print_function);
}