/*  Use for paralel blocking and slow I/O operations 
    Threads should be created and joined
    Mutex created using with PHTHREAD MUTEX INITILIAZER and wrapped for sucess
    compile: gcc -o main main.c -Wall -pthread
 */
#include <pthread.h>
#include <sys/stat.h> // file stats
#include <sys/mman.h> //mmap 
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

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

int main (int argc, char* argv[]) {
    int file_descriptor;

    /* Info about file */
    struct stat stat_file;
    int check_status_file;
    size_t size_file;
    const char * file_name = "foo.txt";
    const char * mapped;

    file_descriptor = open("foo.txt", O_RDONLY);
    check (file_descriptor < 0, "open %s failed: %s", file_name, strerror(errno));

    check_status_file = fstat(file_descriptor, &stat_file);
    check (check_status_file < 0, "stat %s failed: %s", file_name, strerror(errno));
    size_file = stat_file.st_size;

    mapped = mmap(0, size_file, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
    check (mapped == MAP_FAILED, "mmap %s failed: %s", file_name, strerror(errno));

    for (int i = 0; i < size_file; i++) {
        char c;

        c = mapped[i];
        putchar(c);
    }
    return 0;
}