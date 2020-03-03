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
	pthread_cond_t cond_request;
	pthread_cond_t cond_resolve;

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