/**
 * FILENAME :       string.c
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
 *  Collection of string functions.
 */

#include "string.h"
#include "low_level.h"

/**
 * returns the length of a string
 *
 * @param string String to get length of
 * @return int length of string
 */
int strlen(const char *string)
{
    int count = 0;
    while (string[count] != 0)
    {
        count++;
    }
    return count;
}

/**
 * Count the number of occurences of a char in a string
 *
 * @param string
 * @param c
 * @return int
 */
int string_count_char(char *string, char c)
{
    int count = 0;
    int i = 0;
    while (string[i] != 0)
    {
        if (string[i] == c)
        {
            count++;
        }
        i++;
    }
    return count;
}

/**
 * Reduces consecutive occurrences of a char in a string to a single char.
 * Example:
 * "Helllo Worlld" -> "Helo World"
 * The shortening of the string will be accomondated by filling with zeroes
 * from the end.
 *
 * @param string to be changed
 * @param c char to be reduced
 */
void reduce_consecutive_occurrences(char *string, char c)
{
    // points to the \0 at the end
    int last_index = strlen(string);
    int i = 0;
    while (string[i] != 0)
    {
        if (string[i] == c && string[i + 1] == c)
        {
            /**
             * copies the whole part after the second 'c' including the \0 to
             * the current position of the second 'c'.
             * This removes the second 'c' and shortens the string by 1
             */
            memcpy((unsigned char *)(string + i + 1),
                   (unsigned char *)(string + i + 2), last_index - i - 1);
            last_index--;
        }
        else
        {
            i++;
        }
    }
}

/**
 * Copies the content of a 0 terminated string
 *
 * @param source source string
 * @param destination destination string
 */
void string_copy(char *source, char *destination)
{
    int i = 0;
    while (source[i] != 0)
    {
        destination[i] = source[i];
        i++;
    }
}

/**
 * Checks if two strings contain the same chars
 *
 * @param a first string
 * @param b second string
 * @return true if the strings are equal
 * @return false if the strings are not equal
 */
bool string_equals(char *a, char *b)
{
    int index = 0;
    while (true)
    {
        if (a[index] != b[index])
        {
            return false;
        }
        if (a[index] == '\0')
        {
            return true;
        }
        index++;
    }
}

/**
 * Finds first occurence of a char in a string.
 *
 * @param string String to search through
 * @param mark char to find
 *
 * @return index of the first occurrence of mark in string
 */
int string_first(char *string, char mark)
{
    int i = 0;
    while (string[i] != mark)
    {
        if (string[i] == 0)
        {
            return -1;
        }
        i++;
    }
    return i;
}
