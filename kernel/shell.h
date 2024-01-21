/**
 * FILENAME :       shell.h
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
 *  Interface for shell
 */

#ifndef SHELL_C
#define SHELL_C

void start_shell();
void register_command(char *name, int (*function)(int argc, char **argv));
#endif