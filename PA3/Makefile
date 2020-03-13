CC=gcc
CFLAGS=-pthread -Wall -Wextra


multi-lookup: application.o util.o application.h
	$(CC) $(CFLAGS) -o multi-lookup application.o util.o

clean:
	rm *.o
	rm *.txt
	rm multi-lookup
	clear

val:
	valgrind ./multi-lookup 5 5 serviced.txt results.txt input/names1.txt input/names2.txt input/names3.txt input/names4.txt input/names5.txt

test:
	./multi-lookup 5 5 serviced.txt results.txt input/names1.txt input/names2.txt input/names3.txt input/names4.txt input/names5.txt
