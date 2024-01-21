/**
 * FILENAME :       fibonacci.c
 *
 * AUTHOR :         Ruben Lohberg
 *
 * START DATE :     07 Jan 2024
 *
 * LAST UPDATE :    07 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  An external function which will be loaded onto the data-floppy before
 *  (or while) the Operating system is running.
 *  Calculates the fibonacci number at index given via parameter.
 *  Returns result via exit value.
 */

// function declaration
int fib_recursive(unsigned int previous,
                  unsigned int current,
                  unsigned int count);
int string_to_unsigned_int(char *string);

// the first function encountered will be run
int function_main(int argc, char **argv)
{
    if (argc < 2)
    {
        return -1;
    }
    char *input = argv[1];
    unsigned int count = string_to_unsigned_int(input);

    return fib_recursive(0, 1, count);
}

int fib_recursive(unsigned int previous,
                  unsigned int current,
                  unsigned int count)
{
    if (count == 0)
    {
        return previous;
    }
    return fib_recursive(current, previous + current, count - 1);
}

int string_to_unsigned_int(char *string)
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
            return -1;
        }
        result += string[i];
        i++;
    }
    return result;
}
