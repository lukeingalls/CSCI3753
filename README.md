### Luke Ingalls
###### 109019743, luin3949@colorado.edu

1. The *Makefile* is the file that is used to construct the kernel. This is used with the steps listed in the writeup each time a new system call is defined.
1. *cs3753_add.c* is the definition of the system add protocol. It is fairly straightforward except that the *copy_to_user* is needed to make it so the memory references can be made without crossing over user/kernel space.
1. *syscall_64.tbl* is the look up table for each syscall. The **helloworld** function is **333** and the **cs3753_add** is **334**
1. *syscalls.h* has the definitions of each function.
1. The *syslog* is a copy of my syslog demonstrating the functionality of the add function. (It is a few lines up within that file command+f 55, 25, or 30 to potentially find it faster)
1. *test.c* is a program that will show that the add function works by adding 25 and 30 together. Modifications can be made to test other numbers. 


The normal paths for these files are as follows (the relative path begins from the linux directory):
 ```
 ./arch/x86/kernel/cs3753_add.c
 ./arch/x86/kernel/Makefile
 ./arch/x86/entry/entry/syscalls/syscall_64.tbl
 ./include/linux/syscalls.h

 /var/log/syslog
```
The test.c file can go wherever you'd like.
