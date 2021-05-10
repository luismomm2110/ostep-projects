#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
/* error function */
void errorMsg() {
	char error_message[30] = "An error has ocurred\n";

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
	if (numArgs != 2) {
		if (chdir(path) != -1) {
			chdir(path);
		}
	}
}

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

	while (1) {	
		printf("wish> ");
		fflush(stdout);

		char *line = NULL;
		size_t len = 0;		
		char *myargv[20];
		char *path[100]; 
		
		getline(&line, &len, stdin);

		int lenArray =	processInput(line, myargv);

		if (strcmp(myargv[0], "exit") == 0) {
			exitFunction(lenArray);
		}	
		
		if (strcmp(myargv[0], "cd") == 0) {
			cdFunction(lenArray, myargv[1]);
		}

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
			rc = (int) wait(NULL);	
			free(line);
		}
	}

	return 0;

}
