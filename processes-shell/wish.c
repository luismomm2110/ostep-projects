#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {		
	char error_message[30] = "An error has ocurred\n";

	while (1) {	
		printf("wish> ");
		fflush(stdout);

		int i = 0;
		char *line = NULL;
		size_t len = 0;		
		char *myargv[20];
		char *path[100]; 
		char *sliceLine;
		
		getline(&line, &len, stdin);
		line[strlen(line) - 1] = 0;

		while ( (sliceLine = strsep(&line, " ")) != NULL) {
			myargv[i] = strdup(sliceLine);
			i++;
		}

		myargv[i]= NULL;

		if (strcmp(myargv[0], "exit") == 0) {
			exit(0);
		}

		int rc = fork();

		if (rc == 0) {
			
			path[0] = strdup("/bin/");
			strcat(path[0], myargv[0]);

	      		if ((access(path[0], X_OK)) == -1) {
				write(STDERR_FILENO, error_message,
					strlen(error_message));
			}				
				
			myargv[0] = strdup(path[0]);
		
			execv(myargv[0], myargv); 

		} else {
			rc = (int) wait(NULL);	
			free(line);
		}
	}
}

