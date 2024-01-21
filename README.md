# RubenOS

Custom x86 32-Bit Protected Mode operating system and bootloader completely from
scratch. 

Made by Ruben Lohberg as a student project at University of Applied Sciences
Bingen am Rhein.

### Goals and Design Philosophy

Zero external libraries, not even compiler libraries.
- A simple kernel design, propably monolithic
- End Goal: Running a C program from a virtual drive inside my os, which reads
  input parameters, does something with then and outputs either to the console
  or directly to the screen
- Deadline for development: January, 26th 2024 (after three months of
  development + two Months of prior research)
- Then until the end of February: Crating two video presenations:
    - One about assembly and how I used it in development. Goal is to motivate
    beginning students for assembly and show aspects of its relevance in today's
    world and the forseeable future.
    - One about C and operating systems design, for slightly more advanced
    students to motivate them for C and operating systems design and
    development.

### Project steps:
- Working through "Operating Systems: From 0 to 1" by Do Hoang Tu until 7.5,
  which includes reading a few hundred pages of the "Intel 64 and IA-32
  Architectures Software Developer's Manual" and learing about x86 assembly, and
  a bit about gcc and gdb.
- Learning about x86 interrupts and "Ralf Brown's Interrupt List"
- Writing a "Hello World" bootloader using x86 assembly and BIOS interrupt
  calls, and running it on QEMU inside an Ubuntu WSL
- Writing a primitive bootloader, which runs a simple x86 program
- Continuing reading "Operating System: From 0 to 1" and the GNU documentation
  to learn about make and makefiles to automate building
- Finishing "Operating System: From 0 to 1" and realizing it doesn't get to
  properly developing a os in C
- Working through "Writing a Simple Operating System - from Scratch" by Nick
  Blundell from University of Birmingham
- Writing a bootloader that can swtich to 32 bit protected mode
- Writing a "Hello World" os in c and the needed scripts and small programs to
  properly link and load my OS from my bootloader
- Reading "JamesM's kernel development tutorials" to learn about memory mapped
  output
- Implementing screen support using Direct Memory Access
- Reading "Bran's kernel development tutorials" to learn about Keyboard support
- Implementing Interrupt Descriptor Table (IDT), Interrupt Service Routines
  (ISR), and Interrupt Reqeusts (IRQ)
- Implementing Timer and Keyboard support via interrupts
- Redoing screen, interrupt and keyboard code
- Researching Kernel designs, System calls etc.
- Implementing user input via a primitive shell
- Implementing a bad little Floppy disk driver
- Implementing a custom primitive file system on floppy
- Fixing memory overflow issue by rebuilding the entire project by doing the
"Bare Bones" tutorial on OSdev.wiki
    - Setting up a Ubuntu VM in virtualbox instead of wsl
    - Building a cross compiler: i686-elf-gcc
    - Using GRUB bootloader instead of my own one
    - transferring my sources from there on out
    - writing a new makefile
    - getting gdb to work flawlessly (huge help)
- Researched proper driver construction for Floppy and ATA PIO
    - Realized disk drivers can get incredibly complicated and would be a whole
    project of its own
- Using workarounds to get floppy driver to work for my use case
- Got code to run from the floppy disk
- Expanding functionality of running simple c code
- Added directory 'external-functions' which allows you to just put c sources
  in there and they will be compiled to the file_system.h file format and added
  to the data-floppy
- Adding interesting functions for testing and demonstration (fibonacci)
- Adding an external demonstration function, which prints something to the screen
  (print_at)
- Try to reintroduce custom bootloader
- Used newly acquired knowlege to debug the issues with my bootloader and got
  the new version of the os working with my old bootloader. No more GRUB!
- Cleanup and small pretty features


**The current Version**

The current version is v13. It allows the creation, printing and execution of
files.

For info about running external C programs inside RubenOS, see the README.md
file inside the 'external-functions' directory

Navigate to the version's directory and run 'make help' for a helpful message

**Testing the OS in QEMU**

Information valid for v10+

I develop and test the OS in a Ubuntu VM running in Virtualbox / in WSL.
Inside either ubuntu or WSL, I run the OS on the QEMU virtual machine.

For your convenience, I wrote makefiles which do all of the compiling and
running for you. Just navigate to  the "versions" directory and into one of the
versioins "vx" and run:

    make run

You will have to have qemu-system-i386 installed

Alternatively you can just get the os-image.bin and data-floppy files and run
this command directly:

    qemu-system-i386 os-image -drive file=data-floppy format=raw,index=0,if=floppy

### Debugging with gdb

To debug a version run it in debug mode by running:

    make debug

When the virtual machine is running, you can open another terminal and start gdb
from the version's directory. This enables you to use the predetermined
configurations I set up in the version's .gdbinit file. 
From there on you can walk through the instructions with:

    ni

Or define breakpoints, for example:

    b *0xABCD

or

    b main

And continue to them with:

    c
 
### Reading Ralf Brown's Interrupt List:
The list can be dowloaded from http://www.cs.cmu.edu/~ralf/files.html
To simplify reading the list, the COMBINE.COM program can be used (in an 16-Bit
enviroment. I used DOS-BOX) to combine the INTERRUP files into one list which
can be read with RBILVIEW.exe
The "interrupts" directory contains an already combied "INTERRUP.LST" file. When
running the RBIL Viewer (RBILVIEW.exe) the "open List" dialog can be used to
browse the .LST file



### "This is where the real game begins"
