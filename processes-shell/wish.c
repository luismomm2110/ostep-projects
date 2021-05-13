#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


/* error function */
void errorMsg() {
	char error_message[30] = "An error has occurred\n";

	write(STDERR_FILENO, error_message,
	strlen(error_message));
}

/* exit built-in function */
int exitFunction(int numArgs) {
	if (numArgs == 1) {
		exit(0);	
	}
	return 0;
}

void cdFunction(int numArgs, char *path) { 
//	char s[100];
/*	printf("%s\n", getcwd(s, 100));
	chdir("..");
	printf("%s\n", getcwd(s, 100)); */
}
//TODO
/* process input slicing the lines and inserting in myargv */
int processInput(char *userLine, char *userArgs[]) {
	char *sliceLine;
	int i = 0;

	userLine[strlen(userLine)-1] = 0;
	
	
	while ( (sliceLine = strsep(&userLine, " ")) != NULL) {
		userArgs[i] = strdup(sliceLine);
		i++;
	}

	userArgs[i]= NULL;

	return i;
}

int main(int argc, char *argv[]) {		
	char *line = NULL;
	size_t len = 0;		
	char *myargv[20];
	char *path[100]; 
	FILE *fp = stdin;
	ssize_t nread;
	int statval;

	if (argc == 2) {
		if ( (fp = fopen(argv[1], "r")) == NULL) {
			errorMsg();	
		}
	} 

	while (1) {	
		if (argc == 1) {

			printf("wish> ");
			fflush(stdout);
		}	

               if ((nread = getline(&line, &len, fp)) == -1) {
			exit(0);
		}

		int lenArray =	processInput(line, myargv);

		if (strcmp(myargv[0], "exit") == 0) {
			exitFunction(lenArray);
		}	
		
		if (strcmp(myargv[0], "cd") == 0) {
			cdFunction(lenArray, myargv[1]);
		}
		
		char *debugMessage = "write message for debug in STDERR\n";
		
		write(STDERR_FILENO, debugMessage, strlen(debugMessage));
			
		int rc = fork();
		

		if (rc == 0) {
			path[0] = strdup("/bin/");
			strcat(path[0], myargv[0]);

	      		if ((access(path[0], X_OK)) == -1) {
				errorMsg();
				exit(0);
			}				
				
			myargv[0] = strdup(path[0]);
			
			execv(myargv[0], myargv);

		} else {
			wait(&statval);
 			if (WIFEXITED(statval)) {
				if (WEXITSTATUS(statval) != 0) {	
					char *debugError = strerror(WEXITSTATUS(statval));
					write(STDERR_FILENO, debugError,
					strlen(debugError));
				}
			} else {
				printf("not terminate with exit\n");
			}
			free(line);
			line = NULL;
		}
	}
	free(line);
	fclose(fp);
	return 0;
}
