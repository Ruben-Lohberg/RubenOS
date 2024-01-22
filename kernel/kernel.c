/**
 * FILENAME :       kernel.c
 *
 * AUTHOR :         Ruben Lohberg
 *
 * START DATE :     17 Oct 2023
 *
 * LAST UPDATE :    07 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  Getting the ball rolling...
 *  The function kernel_main is the entry to the operating system. Start reading
 *  here if you want to understand how it is working.
 */

#include "screen.h"
#include "idt.h"
#include "irq.h"
#include "timer.h"
#include "keyboard.h"
#include "string.h"
#include "shell.h"
#include "floppy.h"
#include "file_system.h"

/**
 * Test shell command.
 *
 * @param arg Argument string. None expected
 */
static int test_command(int argc, char **argv)
{
    // getting rid of suppressed parameter warnings
    (void)(argc);
    (void)(argv);
    print("Test function\n", DEFAULT_COLOR_SCHEME);
    return 0;
}

/**
 * Echo shell command.
 *
 * @param arg Argument string. Expected format:
 * command_name echo_string
 */
static int echo_command(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        print(argv[i], DEFAULT_COLOR_SCHEME);
        print_char(' ', DEFAULT_COLOR_SCHEME);
    }
    print("\n", 0);
    return 0;
}

/**
 * Main function. The entry point to the operating system.
 */
void kernel_main(void)
{

    print_at("Welcome to", 40, 10, DEFAULT_COLOR_SCHEME);
    print_at("Ruben OS", 43, 11, DEFAULT_COLOR_SCHEME);
    print_at("A custom Operating System", 43, 12, DEFAULT_COLOR_SCHEME);
    print_at("\"This is where the real game begins\"", 35, 13, DEFAULT_COLOR_SCHEME);
    print("\n\n\n", 0);
    idt_install();
    irq_install();

    // set interrupt flag -> Allows the processor to respond
    // to maskable interrupts
    __asm__ __volatile__("sti");

    timer_install();
    keyboard_install();

    floppy_install();
    print("Floppy installed\n", DEFAULT_COLOR_SCHEME);

    start_shell();
    register_command("test", test_command);
    register_command("echo", echo_command);
    install_filesystem();

    // looping forever. From here on out everything happens with interrupts
    for (;;)
    {
        asm("hlt");
    }
}