# FILENAME :    .gdbinit
#
# AUTHOR :      Ruben Lohberg
#
# START DATE :  05 Jan 2024
#
# LAST UPDATE : 21 Jan 2024
#
# PROJECT :     RubenOS
#
# DESCRIPTION :
#   This is a GNU Debugger initialization File
#   When you call gdb from this directory, these commands will be applied
#   automatically. The debugger then allows you to traverse the running
#   by stepping through it (command si), or setting breakpoints and continueing
#   to them. You can also print variables and create memory dumps among other
#   features.

# use intel syntax
set disassembly-flavor intel    
# diplay assembly
layout asm  
# display registers
layout reg  
# set architecture
set architecture i8086  
# connect to QUEMU vm via TCP
target remote localhost:26000   
# set breakpoint at beginning of bootloader
b *0x7c00   
# continue to it
c
# load the symbol file in order to show c code
symbol-file ./debug-info
# set a breakpoint at the main function
b kernel_main
# show c code
layout split