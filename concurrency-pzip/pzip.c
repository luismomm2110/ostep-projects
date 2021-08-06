/*  Use for paralel blocking and slow I/O operations 
    Threads should be created and joined
    Should determine number of threads using processor info
    Mutex created using with PHTHREAD MUTEX INITILIAZER and wrapped for sucess
    references: https://stackoverflow.com/questions/32035671/multithreading-file-compress
    https://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf p14
 */

#define _GNU_SOURCE
#include <sys/stat.h> // file stats
#include <sys/mman.h> //mmap 
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "common_threads.h"


void *thread_printer (void *arg) {
    pid_t tid = gettid();
    printf("%s Thread ID: %d\n", (char *) arg, tid);
    return NULL;
}

static void
check (int test, const char * message, ...)
{
    if (test) {
        va_list args;
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        fprintf (stderr, "\n");
        exit (EXIT_FAILURE);
    }
}

int 
open_file(const char *file_name) { 
    int file_descriptor;
    file_descriptor = open(file_name, O_RDONLY);
    check (file_descriptor < 0, "open %s failed: %s", file_name, strerror(errno));
    
    return file_descriptor;  
}

size_t 
check_status_file(int file_descriptor, struct stat stat_file, const char *file_name) {
    int check_status_file = check_status_file = fstat(file_descriptor, &stat_file);
    check (check_status_file < 0, "stat %s failed: %s", file_name, strerror(errno));

    return stat_file.st_size;
}

const char*
create_map(size_t size_file, const char* file_name, int file_descriptor) {
    const char *mapped = mmap(0, size_file, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
    check (mapped == MAP_FAILED, "mmap %s failed: %s", file_name, strerror(errno));

    return mapped;
}

const int MAX = 30;
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;
int buffer[30];
int loops = 10;

void
put(int value) {
    buffer[fill_ptr] = value;
    fill_ptr = (fill_ptr + 1)%MAX;
    count++;
}

int 
get(){
    int tmp = buffer[use_ptr];
    use_ptr = (use_ptr - 1)%MAX;
    count--;
    printf("%d\n", tmp);
    return tmp;
}

pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        Pthread_mutex_lock(&mutex);
        while (count == MAX) 
            Pthread_cond_wait(&empty, &mutex);
        put(i);
        Pthread_cond_signal(&fill);
        Pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        Pthread_mutex_lock(&mutex);
        while (count == 0) 
            Pthread_cond_wait(&fill, &mutex);
        int tmp = get();
        Pthread_cond_signal(&empty);
        Pthread_mutex_unlock(&mutex);
        printf("%d\n", tmp);
    }
    return NULL;
}

int main (int argc, char* argv[]) {
    pthread_t p1, p2; 
    int file_descriptor;
    /* Info about file */
    struct stat stat_file;
    size_t size_file;
    const char * file_name = "foo.txt";
    const char * mapped;

    file_descriptor = open_file(file_name);
    size_file = check_status_file(file_descriptor, stat_file, file_name);
    mapped = create_map(size_file, file_name, file_descriptor);

    for (int i = 0; i < size_file; i++) {
        char c;
        c =  mapped[i];
        
        Pthread_create(&p1, NULL, thread_printer, &c);
        Pthread_create(&p2, NULL, thread_printer, &c);

    }

    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);

    return 0;
}