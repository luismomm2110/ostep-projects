#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {		
		
	while (1) {	
		printf("wish> ");
		fflush(stdout);

		char *line = NULL;
		size_t len = 0;		
		char *myargv[20];
		char *path[100]; 

		path[0] = strdup("/bin/");
		getline(&line, &len, stdin);
		line[strlen(line) - 1] = 0;

		int rc = fork();

		if (rc == 0) {
			strcat(path[0], line);

	      		if ((access(path[0], X_OK)) == -1) {
				printf("Didn't find command\n");
			}				
			
			myargv[0] = strdup("/bin/ls");
			if (strcmp(myargv[0], path[0]) != 0) {
				printf("not equal\n");
			}			
			
			myargv[0] = strdup(path[0]);
			myargv[1] = NULL;			
		
			execv(myargv[0], myargv);	
	
		} else {
			rc = (int) wait(NULL);	
		}
	}
}

