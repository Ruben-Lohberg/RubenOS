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
 *  Interface for the Keyboard driver
 */

#ifndef KEYBOARD_C
#define KEYBOARD_C

void keyboard_install();
void keyboard_set_print_function(void (*function)(char));
void keyboard_set_default_print_function();

#endif