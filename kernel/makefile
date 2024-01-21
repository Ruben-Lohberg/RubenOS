# FILENAME :    makefile
#
# AUTHOR :      Ruben Lohberg
#
# START DATE :  17 Oct 2023
#
# LAST UPDATE : 21 Jan 2024
#
# PROJECT :     RubenOS
#
# DESCRIPTION :
#   Makefile for building the RubenOS kernel.

# handy definitions
BUILD_DIR=../build/kernel
KERNEL_BINARY=$(BUILD_DIR)/kernel.bin

# flags for gcc
CFLAGS = -fno-pic -m32 -ffreestanding -O0

# all files that end in .c
OS_SRCS := $(wildcard *.c) 
# for every enry in OS_SRCS, generate an entry where .c is substituted with .o
OS_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_SRCS)) 


all: dir $(KERNEL_BINARY)

# for a file XYZ.o, you need the file XYZ.c
$(BUILD_DIR)/%.o: %.c 	
	gcc $(CFLAGS) -c $< -o $@ -g
# $< refers to the input %.asm
# $@ refers to the output %.o


$(KERNEL_BINARY): $(OS_OBJS)
# compile kernel_entry.asm, which shall be loaded at the beginning of the kernel
# before the c binaries
	nasm -O0 kernel_entry.asm -f elf -o $(BUILD_DIR)/kernel_entry.o
# link the actual kernel
	ld -o $@ -Tlink.ld -m elf_i386 --oformat binary $(BUILD_DIR)/kernel_entry.o $(OS_OBJS) -g
# copy it as it contains the gdb debug information
	ld -o ../debug-info -Tlink.ld -m elf_i386 $(BUILD_DIR)/kernel_entry.o $(OS_OBJS) -g
	

.PHONY: dir 

# create the build directory
dir:
	mkdir -p $(BUILD_DIR)