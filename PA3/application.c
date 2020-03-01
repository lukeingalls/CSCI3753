#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 5
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH 50

int main(int argc, char *argv[]) {
	int num_request = atoi(argv[1]);
	int num_resolve = atoi(argv[2]);

	// Verify that ints were given for # of requester/resolver threads
	if (num_request == 0 || num_resolve == 0) {
		printf("An invalid argument was used for the requester/resolver thread amounts.")
		exit(0);
	}

	// Make sure that the requester and resolver threads don't exceed upper limit
	num_request = (num_request <= MAX_REQUESTER_THREADS) ? num_request : MAX_REQUESTER_THREADS;
	num_resolve = (num_resolve <= MAX_RESOLVER_THREADS) ? num_resolve : MAX_RESOLVER_THREADS;
	
	// Each of these strings will be used to hold the file names of the requester of resolver thread
	// The FILE objects will hold the actual file structure.
	char * request_log;
	char * resolve_log;
	FILE *request_file;
	FILE *resolve_file;

	// Open/Populate the  variables mentioned above.
	strcpy(request_log, argv[3]);
	strcpy(resolve_log, argv[4]);
	request_file = fopen(request_log, "w");
	resolve_file = fopen(resolve_log, "w");

	int num_input_files = argc - 5;

	if (request_file && resolve_file) {

	} else {
		printf(stderr, "Invalid output file was encountered.");
		exit(0);
	}
}