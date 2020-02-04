#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	int *ptr = (int*) malloc(sizeof(int));

	int x = 25;
	int y = 30;

	syscall(334, x, y, ptr);

	printf("X (%d) + Y (%d) = %d\nresult stored in %p\n", x, y, *ptr, ptr);

	free(ptr);

}


