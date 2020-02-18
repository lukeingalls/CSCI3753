### Luke Ingalls
######109019743, luin3949@colorado.edu
1. The *Makefile* only contains the one line to create the module. The compilation can be done by using `sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules`. This command calls the makefile and uses the directory /lib/modules/uname -r)/build when doing so. The M=$PWD will return The make command to the current directory when it is done. `modules` compiles the modules.
1. The *pa2_character_driver.c* contains the source code for the kernel module. This can be compiled with the command listed in the previous bullet point.
1. The *test_program.c* will test the interaction with the driver file. The implementation is specified by the assignment writeup. Remember that you must `sudo insmod pa2_character_driver.c` before running it. Also, you need to run `sudo mknod -m 777 /dev/pa2_character_driver c 241 0` before using the module.

These files can all exist in the path which they currently reside. Just need to run the commands listed above where applicable.`
