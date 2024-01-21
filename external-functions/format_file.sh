#!/usr/bin/bash

# FILENAME :    format_file.sh
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
#  This shell script takes a .file file which only contains the raw binary
#  of the function and converts it to the format specified in file_system.h

# filepath
FILE=$1
# filename without suffix
NAME=`basename $FILE .file`
# length of the filename
NAMELENGTH=${#NAME}
# writing the name to a temporary file
echo $NAME > temp
# writing a zero bit at the very end of the name, because for some reason the
# shell string is not zero terminated!?
printf '\0' | dd of=temp bs=1 seek=$NAMELENGTH count=1 conv=notrunc
# fillinf up zero bits until we reach the name length of 60
for (( i=0; i<(59 - $NAMELENGTH); i++))
do 
    printf '\0' >> temp
done
# get size of the file
FILESIZE=`expr $(stat -c%s "$FILE") + 1`
# convert file size to hexadecimal with 8 digits and leading zeroes
# converting that to little endian
# concatenating that onto the temp file
printf "%08X" $FILESIZE | tac -rs .. | echo "$(tr -d '\n')" | xxd -r -p >> temp
# adding the actual function  to the temp file
cat $FILE >> temp
# replacing the .file file with the temp file while getting rid of the temp file
mv temp $FILE

