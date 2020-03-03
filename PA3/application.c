#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 5
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH 50
#define MAX_STR_SIZE 50
#define BUFFER_SIZE 1000

struct THREADS {
	pthread_t request_threads[MAX_REQUESTER_THREADS];
	pthread_t resolve_threads[MAX_RESOLVER_THREADS];
};

struct MUTEX {
	pthread_mutex_t mut_buf;
	pthread_mutex_t mut_input[MAX_INPUT_FILES];
	pthread_mutex_t mut_request;
	pthread_mutex_t mut_resolve;

};

struct GLOBALS {
	int num_files;
	FILE *file_array[MAX_INPUT_FILES];
	char file_status[MAX_INPUT_FILES];
	char buf[BUFFER_SIZE][MAX_STR_SIZE];
	int buf_pos;
	FILE * resolve;
	FILE * request;
	struct THREADS thr;
	struct MUTEX muts;
};

void *requester(void *);
void *resolver(void *);
void clean(struct GLOBALS *);
char all_files_read(char * files, int len);


int main(int argc, char *argv[]) {
	if (argc >= 6) {
		struct GLOBALS g;
		// Define the initial buf_pos
		g.buf_pos = 0;

		int check_tex = 0;

		//Create mutexes
		check_tex |= pthread_mutex_init(&g.muts.mut_buf, NULL);
		for (int i = 0; i < 10; i++) {
			check_tex |= pthread_mutex_init(&g.muts.mut_input[i], NULL);
		}
		check_tex |= pthread_mutex_init(&g.muts.mut_request, NULL);
		check_tex |= pthread_mutex_init(&g.muts.mut_resolve, NULL);

		if (check_tex) {
			fprintf(stderr, "One of the mutexes failed to initialize. Aborting.\n");
			exit(0);
		}

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
		char * request_log = argv[4];
		char * resolve_log = argv[3];
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
		for (int i = 0; i < num_input_files; i++) {
			FILE * temp_file = fopen(argv[i + 5], "r");
			if (temp_file) 
				g.file_array[i] = temp_file;
			else
				continue;
		}

		// Set all files to unread
		for (int i = 0; i < 10; i++) {
			g.file_status[i] = 0;
		}

		if (request_file && resolve_file) {
			//Create the request threads
			for (int i = 0; i < num_request; i++) {
				if (pthread_create(&g.thr.request_threads[i], NULL, requester, &g)) {
					fprintf(stderr, "A requester thread failed to open.\n");
				}
			}

			for (int i = 0; i < num_resolve; i++) {
				if (pthread_create(&g.thr.resolve_threads[i], NULL, resolver, &g)) {
					fprintf(stderr, "A resolver thread failed to open.\n");
				}
			}


			// Wait for requester threads
			for (int i = 0; i < num_request; i++) {
				pthread_join(g.thr.request_threads[i], NULL);
			}
			
			// Wait for resolver threads
			for (int i = 0; i < num_resolve; i++) {
				pthread_join(g.thr.resolve_threads[i], NULL);
			}

			// Clean up everything
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
	int counter = 0;
	// Get the thread id
	pid_t threadid = syscall(SYS_gettid);

	// Assign value based on thread number.
	// int j = 0;
	// for (; j < MAX_INPUT_FILES; j++) if ( (unsigned int)threadid == (unsigned int) globals->thr.request_threads[j]) printf("match\n");

	// Num_files doesn't change and thus is thread safe.
	for (int i = 0; i < globals->num_files; i++) {
		if (globals->file_status[i]) {
			continue;
		}
		counter += 1;
		while(1) {
			
			
			// Lock the file we are reading from (or sleep)
			pthread_mutex_lock(&(globals->muts.mut_input[i]));
			if (fgets(line, MAX_STR_SIZE, globals->file_array[i])) {
				// Unlock file being read
				pthread_mutex_unlock(&(globals->muts.mut_input[i]));
			


				line[strlen(line) - 1] = '\0';
				

				//Lock and unlock the buffer
				pthread_mutex_lock(&(globals->muts.mut_buf));
				strncpy(globals->buf[globals->buf_pos], line, MAX_STR_SIZE);
				globals->buf_pos++;
				pthread_mutex_unlock(&(globals->muts.mut_buf));


				// printf("%s\n", line);

			} else {
				// Unlock file being read and terminate the loop
				pthread_mutex_unlock(&(globals->muts.mut_input[i]));
				globals->file_status[i] = 1;
				break;
			}
		}
	}

	// Lock the file as we write to it
	pthread_mutex_lock(&(globals->muts.mut_resolve));
	fprintf(globals->request, "Thread <%d> serviced %d files\n", threadid, counter);
	pthread_mutex_unlock(&(globals->muts.mut_resolve));
	return 0;
}

void *resolver(void * g) {
	struct GLOBALS *globals = (struct GLOBALS *) g;
	char ip[MAX_IP_LENGTH];
	char temp[MAX_STR_SIZE];

	pthread_mutex_lock(&(globals->muts.mut_buf));
	while (!all_files_read(globals->file_status, globals->num_files) || globals->buf_pos > 0) {
		strncpy(temp, globals->buf[globals->buf_pos - 1], MAX_STR_SIZE);
		globals->buf_pos--;
		pthread_mutex_unlock(&(globals->muts.mut_buf));


		if (dnslookup(temp, ip, MAX_IP_LENGTH) == UTIL_FAILURE) {
			printf("%s coud not be resolved.\n", temp);
			pthread_mutex_lock(&(globals->muts.mut_resolve));
			fprintf(globals->resolve, ",\n");
			pthread_mutex_unlock(&(globals->muts.mut_resolve));
		} else {
			pthread_mutex_lock(&(globals->muts.mut_resolve));
			fprintf(globals->resolve, "%s,%s\n", temp, ip);
			pthread_mutex_unlock(&(globals->muts.mut_resolve));
		}

		//Lock the buffer before the while is assessed.
		pthread_mutex_lock(&(globals->muts.mut_buf));
	}
	pthread_mutex_unlock(&(globals->muts.mut_buf));

	return 0;
}

void clean(struct GLOBALS *globals) {
	pthread_mutex_destroy(&(globals->muts.mut_buf));
	for (int i = 0; i < 10; i++) {
		pthread_mutex_destroy(&(globals->muts.mut_input[i]));
	}
	pthread_mutex_destroy(&(globals->muts.mut_request));
	pthread_mutex_destroy(&(globals->muts.mut_resolve));

	fclose(globals->request);
	fclose(globals->resolve);

	for (int i = 0; i < globals->num_files; i++) {
		fclose(globals->file_array[i]);
	}
}

char all_files_read(char * files, int len) {
	char res = 1;
	for (int i = 0; i < len; i++) {
		res &= files[i];
	}

	return res;
}