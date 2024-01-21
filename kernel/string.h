/**
 * FILENAME :       string.h
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
 *  Interface for string functions
 */

#ifndef STRING_C
#define STRING_C

#include "bool.h"

int strlen(const char *string);
bool string_equals(char *a, char *b);
void string_copy(char *source, char *destination);
int string_count_char(char *string, char c);
void reduce_consecutive_occurrences(char *string, char c);
int string_first(char *string, char mark);

#endif