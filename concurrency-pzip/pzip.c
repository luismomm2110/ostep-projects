/*  Use for paralel blocking and slow I/O operations 
    Threads should be created and joined
    Should determine number of threads using processor info
    Mutex created using with PHTHREAD MUTEX INITILIAZER and wrapped for sucess
    references: https://stackoverflow.com/questions/32035671/multithreading-file-compress
    https://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf p14
 */

#define _GNU_SOURCE
#define MAX 4
#define FULL 0
#define EMPTY 0

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
    int check_status_file  = fstat(file_descriptor, &stat_file);
    check (check_status_file < 0, "stat %s failed: %s", file_name, strerror(errno));

    return stat_file.st_size;
}

const char*
create_map(size_t size_file, const char* file_name, int file_descriptor) {
    const char *mapped = mmap(0, size_file, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
    check (mapped == MAP_FAILED, "mmap %s failed: %s", file_name, strerror(errno));

    return mapped;
}

int next_in = 0;
int next_out = 0;
char buffer[MAX];
sem_t empty, full;

void
put(char item) {
    Sem_wait(&empty) //dont put if not empty

    buffer[next_in] = item;
    next_in = (next_in + 1)%MAX;
    
    if (next_in == FULL) {
        Sem_post(&full);
        sleep(1);
    }
    Sem_post(&empty);
}

void 
get(){
    int item;

    Sem_wait(&full);

    item = buffer[next_out];
    next_out = (next_out + 1)%MAX;
    printf("\t...Consuming %c ...nextOut %d..Ascii=%d\n",item,next_out,item);
    
    if (next_out == EMPTY)
        sleep(1);

    Sem_post(&full);
}

void *producer() {
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
        put(c);
    }
    return NULL;
}

void *consumer() {
    int i;
    for (i = 0; i < 10; i++) {
        get();
    }
    return NULL;
}

int main (int argc, char* argv[]) {
    pthread_t p1, p2; 


    Sem_init(&empty, 1);
    Sem_init(&full, 0);

    Pthread_create(&p1, NULL, producer, NULL);
    Pthread_create(&p2, NULL, consumer, NULL);

    /* gave a way of sending this to a thread;
    */ 

    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);

    return 0;
}