#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 5
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH 50
#define MAX_STR_SIZE 50

struct GLOBALS {
	int num_files;
	FILE ** file_array;
	char buf[1000][MAX_STR_SIZE];
	int buf_pos;
	FILE * resolve;
	FILE * request;
};

void requester(struct GLOBALS *);
void resolver(struct GLOBALS *);
void clean(struct GLOBALS *);


int main(int argc, char *argv[]) {
	if (argc >= 6) {
		struct GLOBALS g;
		// Define the initial buf_pos
		g.buf_pos = 0;

		int num_request = atoi(argv[1]);
		int num_resolve = atoi(argv[2]);

		// Verify that ints were given for # of requester/resolver threads
		if (num_request == 0 || num_resolve == 0) {
			fprintf(stderr, "An invalid argument was used for the requester/resolver thread amounts.\n");
			exit(0);
		}

		// Make sure that the requester and resolver threads don't exceed upper limit
		num_request = (num_request <= MAX_REQUESTER_THREADS) ? num_request : MAX_REQUESTER_THREADS;
		num_resolve = (num_resolve <= MAX_RESOLVER_THREADS) ? num_resolve : MAX_RESOLVER_THREADS;
		
		// Each of these strings will be used to hold the file names of the requester of resolver thread
		// The FILE objects will hold the actual file structure.
		char * request_log = argv[3];
		char * resolve_log = argv[4];
		FILE *request_file;
		FILE *resolve_file;

		// Open/Populate the variables mentioned above.
		request_file = fopen(request_log, "w");
		resolve_file = fopen(resolve_log, "w");
		// Put them in the globals struct
		g.request = request_file;
		g.resolve = resolve_file;

		int num_input_files = argc - 5;
		
		// Define the number of files and pass them to the globals struct
		g.num_files = num_input_files;
		g.file_array = (FILE *) malloc(sizeof(FILE *) * num_input_files);
		for (int i = 0; i < num_input_files; i++) {
			FILE * temp_file = fopen(argv[i + 5], "r");
			if (temp_file) 
				g.file_array[i] = temp_file;
			else
				continue;
		}

		if (request_file && resolve_file) {

			requester(&g);
			fclose(request_file);
			fclose(resolve_file);
			clean(&g);
		} else {
			fprintf(stderr, "Invalid output file was encountered.\n");
			clean(&g);
			exit(0);
		}
	} else {
		fprintf(stderr, "Not enough command line args\n");
	}

}

void requester(struct GLOBALS *globals) {
	char line[MAX_STR_SIZE];
	for (int i = 0; i < globals->num_files; i++) {
		while(fgets(line, MAX_STR_SIZE, globals->file_array[i])) {
			line[strlen(line) - 1] = '\0';
			strncpy(globals->buf[globals->buf_pos], line, MAX_STR_SIZE);
			globals->buf_pos++;
			fprintf(globals->resolve, "%s\n", line);
		}
	}
}

void clean(struct GLOBALS *globals) {
	if (globals->file_array) {
		free(globals->file_array);
	}
}