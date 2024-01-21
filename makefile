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
#   Makefile for building the whole project.

# handy definitions
BUILD_DIR=./build
IMAGE=os-image.bin
BOOT_SECTOR_BINARY=$(BUILD_DIR)/bootloader/boot_sect.bin
KERNEL_BINARY=$(BUILD_DIR)/kernel/kernel.bin

# things to build when just running "make" without arguments
all: image external-functions-floppy

# building the actual image by concatenating binaries of the boot sector and
# kernel
image: $(BOOT_SECTOR_BINARY) $(KERNEL_BINARY)
	cat $(BOOT_SECTOR_BINARY) $(KERNEL_BINARY) > $(IMAGE)

# build the bootloader by calling the makefile inside the bootloader dir
$(BOOT_SECTOR_BINARY):
	make -C bootloader

# build the os kernel by calling the makefile inside the os dir
$(KERNEL_BINARY):
	make -C kernel

# build targets that do not represent actual build files. Commands will just be
# run no matter what, when passed as an argument
.PHONY: data-floppy external-functions-floppy run debug build-and-run clean \
$(BOOT_SECTOR_BINARY) $(KERNEL_BINARY)

# A new empty 1.44MB virtual floppy disk
data-floppy:
	dd if=/dev/zero of=data-floppy count=1440 bs=1k

# create a data floppy with external functions from the external-functions
# directory already loaded in the format of the custon file system
external-functions-floppy:
	make -C external-functions

# command to run qemu with the os and the data floppy
RUN_COMMAND=qemu-system-i386 $(IMAGE) -drive \
file=data-floppy,format=raw,index=0,if=floppy

# run normally
run:
	$(RUN_COMMAND)

# run with debugger attached
debug:
	$(RUN_COMMAND) -gdb tcp::26000 -S

# delete build files, rebuild and run
build-and-run: clean all run

# delete build files
clean:
	rm -rf build
	make -C external-functions clean



