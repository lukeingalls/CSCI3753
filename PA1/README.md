### Luke Ingalls
###### 109019743, luin3949@colorado.edu

The Makefile is the file that is used to construct the kernel. cs3753_add.c is the definition of the system add protocol. syscall_64.tbl and syscalls.h both are used when constructing the kernel. The syslog is a copy of my syslog demonstrating the functionality of the add function. test.c is a program that will show that the add function works by adding 25 and 30 together. Modifications can be made to test other numbers. 


The normal paths for these files are as follows (the relative path begins from the linux directory):
./arch/x86/kernel/cs3753_add.c
./arch/x86/kernel/Makefile
./arch/x86/entry/entry/syscalls/syscall_64.tbl
./include/linux/syscalls.h

/var/log/syslog

The test.c file can go wherever you'd like.
