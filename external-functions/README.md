### Info for running external C functions inside RubenOS

You can just create a file XYZ.c inside this directory and the outer makefile
will make sure, that the binary of the code you write gets added to the
data-floppy as a file in the format of the RubenOS file system.

See test_function.c for an example of how such a file can look and what it
can do.

Right now, access to RubenOS functions, like 'print' is not available, but
text can be printet by utilizing direct memory access to the video memory
at address 0xb8000