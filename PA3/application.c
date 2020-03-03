#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 5
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH 50
#define MAX_STR_SIZE 50


struct THREADS {
	pthread_t request_threads[10];
	pthread_t resolve_threads[10];
};

struct MUTEX {
	pthread_mutex_t mut_buf;
	pthread_mutex_t mut_input[10];
	pthread_mutex_t mut_request;
	pthread_mutex_t mut_resolve;

};

struct GLOBALS {
	int num_files;
	FILE ** file_array;
	char buf[1000][MAX_STR_SIZE];
	int buf_pos;
	FILE * resolve;
	FILE * request;
	struct THREADS thr;
	struct MUTEX muts;
};

void *requester(void *);
void *resolver(struct GLOBALS *);
void clean(struct GLOBALS *);


int main(int argc, char *argv[]) {
	if (argc >= 6) {
		struct GLOBALS g;
		// Define the initial buf_pos
		g.buf_pos = 0;

		//Create mutexes
		pthread_mutex_init(&g.muts.mut_buf, NULL);
		for (int i = 0; i < 10; i++) {
			pthread_mutex_init(&g.muts.mut_input[i], NULL);
		}
		pthread_mutex_init(&g.muts.mut_request, NULL);
		pthread_mutex_init(&g.muts.mut_resolve, NULL);

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
		g.file_array = (FILE **) malloc(sizeof(FILE *) * num_input_files);
		for (int i = 0; i < num_input_files; i++) {
			FILE * temp_file = fopen(argv[i + 5], "r");
			if (temp_file) 
				g.file_array[i] = temp_file;
			else
				continue;
		}

		if (request_file && resolve_file) {
			//Create the request threads
			for (int i = 0; i < num_request; i++) {
				if (pthread_create(&g.thr.request_threads[i], NULL, requester, &g)) {
					fprintf(stderr, "A requester thread failed to open.\n");
				}
			}

			// Wait for requester threads
			for (int i = 0; i < num_request; i++) {
				pthread_join(g.thr.request_threads[i], NULL);
			}
			// resolver(&g);
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

void *requester(void *g) {
	struct GLOBALS *globals = (struct GLOBALS *) g;
	char line[MAX_STR_SIZE];
	for (int i = 0; i < globals->num_files; i++) {
		while(fgets(line, MAX_STR_SIZE, globals->file_array[i])) {
			line[strlen(line) - 1] = '\0';
			strncpy(globals->buf[globals->buf_pos], line, MAX_STR_SIZE);
			globals->buf_pos++;
			fprintf(globals->resolve, "%s\n", line);
		}
	}

	return 0;
}

void *resolver(struct GLOBALS *globals) {
	char ip[MAX_IP_LENGTH];
	for (int i = globals->buf_pos - 1; i >= 0; i--) {
		if (dnslookup(globals->buf[i], ip, MAX_IP_LENGTH) == UTIL_FAILURE) {
			printf("%s could not be resolved.\n", globals->buf[i]);
			fprintf(globals->request, ",\n");

		} else {
			fprintf(globals->request, "%s,%s\n", globals->buf[i], ip);
		}
	}

	return 0;
}

void clean(struct GLOBALS *globals) {
	if (globals->file_array) {
		free(globals->file_array);
	}
}