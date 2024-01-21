# FILENAME :    link.ld
#
# AUTHOR :      Ruben Lohberg
#
# START DATE :  Dec 03 2023
#
# LAST UPDATE : Jan 21 2024
#
# PROJECT :     RubenOS
#
# DESCRIPTION :
#   Linker script to properly link the binaries for execution on bare metal

ENTRY(_start)
SECTIONS
{
  .text 0xf000 :
    {
        *(.text)
    }

}