#!/usr/bin/bash

# FILENAME :    fill_floppy.sh
#
# AUTHOR :      Ruben Lohberg
#
# START DATE :  06 Jan 2024
#
# LAST UPDATE : 06 Jan 2024
#
# PROJECT :     RubenOS
#
# DESCRIPTION :
#  This shell script takes all the .file files of the external functions and
#  copies their binary to to the correct place in the data-floppy while also
#  generating a valid corresponding file record at the begginging of the floppy.
#  These formats are defined in filesystem.c 

# track the index of the file we're currently working with
FILE_INDEX=0
# size of a floppy track. 
MEMORY_SIZE=18432
# memory size for a filename
FILENAME_SIZE=60

# iterate over parameters (paths to .file files)
for i
do
    # current file path
    FILE=$i
    # name of the current file without suffix
    NAME=`basename $FILE .file`
    # length of the name
    NAMELENGTH=${#NAME}
    # index in memory where the name needs to be placed inside the record
    MEMORYINDEX=$((FILE_INDEX * FILENAME_SIZE +2))
    # writing the name to the names array inside the file record
    echo $NAME | dd of=../data-floppy bs=1 seek=$MEMORYINDEX count=$NAMELENGTH conv=notrunc
    # incrementing the file index
    ((FILE_INDEX++))
    # size of the .file file
    FILESIZE=`expr $(stat -c%s "$FILE") + 1`
    # place in memory where we want to place the contents of the file
    MEMORYINDEX=$((FILE_INDEX * MEMORY_SIZE))
    # placing the .file inside the data.floppy
    dd if=$FILE of=../data-floppy bs=1 seek=$MEMORYINDEX count=$FILESIZE conv=notrunc
done

# taking the last file index as the number of files we placed.
# converting that to hex with 4 digits and leading zeroes
# converting that to little endian
# placing that at the very beginning of the floppy as the file count
printf "%04X" $FILE_INDEX | tac -rs .. | echo "$(tr -d '\n')" | xxd -r -p | dd of=../data-floppy bs=1 seek=0 count=4 conv=notrunc