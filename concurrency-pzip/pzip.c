/*  Use for paralel blocking and slow I/O operations 
    Threads should be created and joined
    Should determine number of threads using processor info
    Mutex created using with PHTHREAD MUTEX INITILIAZER and wrapped for sucess
    references: https://stackoverflow.com/questions/32035671/multithreading-file-compress
    https://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf p14
 */

#define _GNU_SOURCE
#define MAX 1
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

char
*concat_char(int number, char c) {
    char *item = malloc(sizeof(char)*5);
    sprintf(item, "%d%c", number, c);
    return item;
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
    int check_status_file  = fstat(file_descriptor, &stat_file);
    check (check_status_file < 0, "stat %s failed: %s", file_name, strerror(errno));
    return stat_file.st_size;
}

const char*
create_map(size_t size_file, const char* file_name, int file_descriptor) {
    const char *mapped = mmap(0, size_file, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
    return mapped;
}

void
write_file(char* item, FILE *fp)
{
    fputs(item, fp);
}

int next_in = 0;
int next_out = 0;
char* buffer[MAX];
sem_t empty, full;

void
put(char* item) {
    //dont put if not empty, in first value of empty is one so first threads passes
    Sem_wait(&empty);

    //put item in current position of buffer
    buffer[next_in] = item;
    printf("In put: %s\n", item);
    next_in = (next_in + 1)%MAX;
    if (next_in == FULL) {
        //if full, signal that some thread can pass full
        Sem_post(&full);
        sleep(1);
    }
    //restore value of empty so other thread can enter
    Sem_post(&empty);
}

char  
*get(){
    char* item;

    // init at zero, can only enter after buffer was fulled by producer
    Sem_wait(&full);

    item = buffer[next_out];
    printf("in get %s\n", item);
    next_out = (next_out + 1)%MAX;
    if (next_out == EMPTY)
        sleep(1);

    //restore post to one so wake up other thread waiting 
    Sem_post(&full);
    Sem_wait(&full);
    return item;
}

void *producer() {
    int file_descriptor;
    /* Info about file */
    struct stat stat_file;
    size_t size_file;
    const char * file_name = "foo.txt";
    const char * mapped;
    char* item;

    file_descriptor = open_file(file_name);
    size_file = check_status_file(file_descriptor, stat_file, file_name);
    mapped = create_map(size_file, file_name, file_descriptor);

    char initial_char = mapped[0];
    int count = 1;
    item = concat_char(count, initial_char);

    for (int i = 0; i < size_file; i++) {
        char c =  mapped[i];
        if (c == initial_char) {
            count++;
        } else {
            if (c != '\0') { 
                item = concat_char(count, initial_char);
                put(item);
            }
            count = 1; 
            initial_char = c;
        }
    }
    return NULL;
}

void *consumer(void* fp) {
    int i;
    char* item;

    fp = (FILE* ) fp;

    for (i = 0; i < 5; i++) {
        item = get();
        write_file(item, fp);
    }
    return NULL;
}

int main (int argc, char* argv[]) {
    pthread_t p1, p2; 
    FILE *fp;

    fp = fopen("result.txt", "w+");
    if (fp < 0)
        printf("error opening file\n");

    Sem_init(&empty, 1);
    Sem_init(&full, 0);

    Pthread_create(&p1, NULL, producer, NULL);
    Pthread_create(&p2, NULL, consumer, (void *) fp);
    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);

    fclose(fp);

    return 0;
}
