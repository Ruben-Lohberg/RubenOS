/**
 * FILENAME :       test_function.c
 *
 * AUTHOR :         Ruben Lohberg
 *
 * START DATE :     06 Jan 2024
 *
 * LAST UPDATE :    07 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  An external function which will be loaded onto the data-floppy before
 *  (or while) the Operating system is running.
 */

// local declarations can be made before main function
void print(unsigned int i, char c);

// the first function encountered will be run
int function_main(int argc, char **argv)
{
    unsigned char *videoMemory = (unsigned char *)0xb8000;
    print(160, 'H');
    print(161, 'e');
    print(162, 'l');
    print(163, 'l');
    print(164, 'o');
    for (unsigned int i = 0; i < argc; i++)
    {
        print(i + 165, argv[i][0]);
    }
    return argc;
}

void print(unsigned int i, char c)
{
    unsigned char *videoMemory = (unsigned char *)0xb8000;
    videoMemory[i * 2] = c;
    videoMemory[i * 2 + 1] = 0x0f;
}
