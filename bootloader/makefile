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
#   Makefile for building the RubenOS bootloader.

# handy definitions
BUILD_DIR=../build/bootloader
BOOT_SECT_BINARY=$(BUILD_DIR)/boot_sect.bin


all: dir $(BOOT_SECT_BINARY)

# compile the bootloader witrh asm
$(BOOT_SECT_BINARY):
	 nasm -O0 boot_sect.asm -f bin -o $(BOOT_SECT_BINARY)

.PHONY: dir

# create build directory
dir:
	mkdir -p $(BUILD_DIR)