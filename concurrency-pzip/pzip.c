/*  Use for paralel blocking and slow I/O operations 
    Threads should be created and joined
    Mutex created using with PHTHREAD MUTEX INITILIAZER and wrapped for sucess
 */
#include <sys/stat.h> // file stats
#include <sys/mman.h> //mmap 
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "common_threads.h"

void *thread_printer (void *arg) {
    printf("%s\n", (char *) arg);
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
    int file_descriptor = 0;
    file_descriptor = open(file_name, O_RDONLY);
    check (file_descriptor < 0, "open %s failed: %s", file_name, strerror(errno));
    
    return file_descriptor;  
}


int main (int argc, char* argv[]) {
    pthread_t p1, p2; 
    int file_descriptor;
    /* Info about file */
    struct stat stat_file;
    int check_status_file;
    size_t size_file;
    const char * file_name = "foo.txt";
    const char * mapped;

    file_descriptor = open_file(file_name);

    check_status_file = fstat(file_descriptor, &stat_file);
    check (check_status_file < 0, "stat %s failed: %s", file_name, strerror(errno));
    size_file = stat_file.st_size;

    mapped = mmap(0, size_file, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
    check (mapped == MAP_FAILED, "mmap %s failed: %s", file_name, strerror(errno));

    Pthread_create(&p1, NULL, thread_printer, "A");
    Pthread_create(&p2, NULL, thread_printer, "B");

    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);

    /*
    for (int i = 0; i < size_file; i++) {
        char c;

        c = mapped[i];
        putchar(c);
    }
    */
    return 0;
}