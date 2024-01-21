# FILENAME :    makefile
#
# AUTHOR :      Ruben Lohberg
#
# START DATE :  06 Jan 2024
#
# LAST UPDATE : 21 Jan 2024
#
# PROJECT :     RubenOS
#
# DESCRIPTION :
#  Makefile for building external functions loading then onto a data-floppy
#  as files in the defined in file_system.c

# handy definitions
BUILD_DIR=./build
DATA_FLOPPY=../data-floppy

# all files that end in .c
C_SRCS := $(wildcard *.c) 
# for every entry in C_SRCS, generate an entry where .c is substituted with .o
C_OBJS := $(patsubst %.c, $(BUILD_DIR)/c/%.o, $(C_SRCS))
# for every entry in C_OBJS, generate a raw binary
RAW_BINS := $(patsubst $(BUILD_DIR)/c/%.o, $(BUILD_DIR)/raw/%.bin, $(C_OBJS))
# for every entry in RAW_BINs, generate a file for ROS
ROS_FILES := $(patsubst $(BUILD_DIR)/raw/%.bin, $(BUILD_DIR)/file/%.file, $(RAW_BINS))

# don't delete those files, as they might be interesting to look at
.PRECIOUS: $(RAW_BINS) $(C_OBJS)

# recompile everything and build a new floppy
all: $(BUILD_DIR) filled_floppy

# create the build direcotry
$(BUILD_DIR):
	mkdir -p $@/c
	mkdir -p $@/raw
	mkdir -p $@/file

# compile the c files
$(BUILD_DIR)/c/%.o: %.c 
	gcc -e function_main -fno-pic -m32 -ffreestanding -O0 -nostdlib -lgcc $< -o $@

# extract only the code in the .text sections from the binary
$(BUILD_DIR)/raw/%.bin: $(BUILD_DIR)/c/%.o
	objcopy -O binary --only-section=.text $< $@

# generate a .file function file in the format of file_system.h
$(BUILD_DIR)/file/%.file: $(BUILD_DIR)/raw/%.bin
	cat $< > $@
	./format_file.sh $@

.PHONY: $(DATA_FLOPPY) clean filled_floppy

# create a new floppy
$(DATA_FLOPPY):
	dd if=/dev/zero of=$(DATA_FLOPPY) count=1440 bs=1k

# fill the floppy with function files
filled_floppy: $(DATA_FLOPPY) $(ROS_FILES)
	./fill_floppy.sh $(ROS_FILES)

# remove build files
clean:
	rm -rf $(BUILD_DIR)