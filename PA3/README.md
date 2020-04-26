# PA3

The program can be run by doing 
1. `make`
1. `make test`

This will run the program with 5 requester threads, 5 resolver threads and all the input files.

To run this with custom values just run

`./multi-lookup #REQUESTERS #RESOLVERS requester_log resolver_log input_files1 ...`

`make clean` can be run to clean up the base directory
`make val` runs the test described above with valgrind

The `performance.txt` file describes outputs of running with specific requeseter, resolver quantities.
`application.c` houses the main driver of the program and `application.h` houses the definitions of values and then the structure definitions and the function protoypes. The `util` files are the given code to make the resolving happen.
