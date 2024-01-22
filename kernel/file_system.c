/**
 * FILENAME :       file_system.c
 *
 * AUTHOR :         Ruben Lohberg
 *
 * START DATE :     16 Dec 2023
 *
 * LAST UPDATE :    07 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  A primitive custom file system for floppy drives. Actual file systems get
 *  way more complicated than this.
 */

#include "file_system.h"
#include "floppy.h"
#include "screen.h"
#include "string.h"
#include "shell.h"
#include "low_level.h"

/**
 * Maximal number of Files a 1.44MB floppy can hold, because each file
 * has a dedicated track, which there are 120 of. Track 0 is reserved for the
 * files record
 * */
#define MAX_FILE_COUNT 119
/** Track file index 0 is reserverd for the files record */
#define FILES_RECORD_INDEX 0

/**
 * Record of the files on the drive. Containing count and names
 */
typedef struct record
{
    // amount of files currently on the drive
    unsigned short file_count;
    // array of file names
    char file_names[MAX_FILE_COUNT][MAX_FILENAME_LENGTH];
} __attribute__((packed)) record;

/**
 * Create a file, which is then written to the floppy.
 *
 * @param filename Zero-Terminated String. Name of the file. Maximum 59 chars
 * @param data Zero-Terminated String. Content of the file. Maximum 18367 chars
 */
void create_file(char *filename, char *data)
{
    // load the file record
    floppy_read_buffer(FILES_RECORD_INDEX);
    struct record *record = (struct record *)floppy_dmabuf;

    // save the filename into the record
    string_copy(filename, record->file_names[record->file_count]);
    // increment the filecount
    record->file_count++;
    int count = record->file_count;

    // write the changed record
    floppy_write_buffer(FILES_RECORD_INDEX);

    // clear the buffer
    floppy_clear_buffer();
    // access the buffer as a file
    struct file *file = (struct file *)floppy_dmabuf;
    // write the filename
    string_copy(filename, file->name);
    // write the data length
    file->data_length = strlen(data) + 1;
    // write the data
    string_copy(data, (char *)file->data);

    // write the file to the next free track on the floppy
    floppy_write_buffer(count);
}

/**
 * Shell command function for creating a new file
 *
 * @param args Arguments string. Expected format:
 * command_name file_name file_data
 */
static int create_file_command(int argc, char **argv)
{
    if (argc < 3)
    {
        print("Error: Did not provide enough arguments!\n",
              DEFAULT_COLOR_SCHEME);
        return 1;
    }
    // Replace zero bits before argument strings with whitespaces to create
    // one long string
    for (int i = 3; i < argc; i++)
    {
        argv[i][-1] = ' ';
    }
    create_file(argv[1], argv[2]);
    return 0;
}

/**
 * Shell command function for listing all files on the floppy
 *
 * @param args Arguments string. None expected
 */
static int list_files_command(int argc, char **argv)
{
    // surpressing unused parameter warnings
    (void)(argc);
    (void)(argv);

    print("Listing files...\n", DEFAULT_COLOR_SCHEME);
    // reading files record
    floppy_read_buffer(FILES_RECORD_INDEX);
    struct record *record = (struct record *)floppy_dmabuf;
    // for each file, print its name
    for (int i = 0; i < record->file_count; i++)
    {
        print(record->file_names[i], DEFAULT_COLOR_SCHEME);
        print("\n", 0);
    }
    return 0;
}

/**
 * Shell command function for printing a file's content
 *
 * @param args Arguments string. Expected format:
 * command_name file_name
 */
static int print_file_command(int argc, char **argv)
{
    if (argc < 2)
    {
        print("Error: Did not provide enough arguments!\n",
              DEFAULT_COLOR_SCHEME);
        return 1;
    }
    // the second word is the filename argument
    char *filename = argv[1];
    // read the files record
    floppy_read_buffer(FILES_RECORD_INDEX);
    struct record *record = (struct record *)floppy_dmabuf;
    // try to find a record entry for the filename
    for (unsigned short i = 0; i < record->file_count; i++)
    {
        if (string_equals(record->file_names[i], filename))
        {
            // read the found file
            floppy_read_buffer(i + 1);
            struct file *file = (struct file *)floppy_dmabuf;
            // print its full data, even if there are \0 bytes
            for (unsigned int j = 0; j < file->data_length; j++)
            {
                print_char(((unsigned char *)(file->data))[j],
                           DEFAULT_COLOR_SCHEME);
            }
            print("\n", DEFAULT_COLOR_SCHEME);
            return 0;
        }
    }
    // print file not found message, if the filename was not found in the record
    print("File: '", DEFAULT_COLOR_SCHEME);
    print(filename, DEFAULT_COLOR_SCHEME);
    print("' not found\n", DEFAULT_COLOR_SCHEME);
    return 0;
}

/**
 * Shell command function for executing a file's content as a function
 *
 * @param args Arguments string. Expected format:
 * command_name file_name
 */
static int execute_file_command(int argc, char **argv)
{
    if (argc < 2)
    {
        print("Error: Did not provide enough arguments!\n",
              DEFAULT_COLOR_SCHEME);
        return 1;
    }
    // the second word is the filename argument
    char *filename = argv[1];
    // read the files record
    floppy_read_buffer(FILES_RECORD_INDEX);
    struct record *record = (struct record *)floppy_dmabuf;
    // try to find a record entry for the filename
    for (int i = 0; i < record->file_count; i++)
    {
        if (string_equals(record->file_names[i], filename))
        {
            // if the file is found read it into the buffer
            floppy_read_buffer(i + 1);
            struct file *file = (struct file *)floppy_dmabuf;
            // execute the data as if it were a file with the signature:
            // int filename(int argc, char **argv);
            int (*func)(int argc, char **argv) = (int (*)(int, char **))file->data;
            // getting rid of the first argv string 'execute'
            argv = &argv[1];
            argc--;
            int exit_value = func(argc, argv);
            print("Program ended with exit value: ", DEFAULT_COLOR_SCHEME);
            print_int(exit_value, DEFAULT_COLOR_SCHEME);
            print("\n", 0);
            return 0;
        }
    }
    // print file not found message, if the filename was not found in the record
    print("File: '", DEFAULT_COLOR_SCHEME);
    print(filename, DEFAULT_COLOR_SCHEME);
    print("' not found\n", DEFAULT_COLOR_SCHEME);
    return 0;
}

/**
 * Installing the file system.
 */
void install_filesystem()
{
    // clear the floppy buffer in case there is something in there
    floppy_clear_buffer();
    // register the commands
    register_command("list", (int (*)(int, char **))list_files_command);
    register_command("create", (int (*)(int, char **))create_file_command);
    register_command("print", (int (*)(int, char **))print_file_command);
    register_command("execute", (int (*)(int, char **))execute_file_command);
}
