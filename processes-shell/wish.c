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
	if (numArgs != 2) {
		errorMsg();	
		exit(0);
	}	
	
	if (chdir(path) == -1) {
		errorMsg();
		exit(0);	
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

//redirect stdout and stdout of children to parent
void redirectChild(FILE *in) {
	int outFile;
	if ((outFile = fileno(in)) == -1) {
		errorMsg();
		return;
	}
	
	if (outFile != STDOUT_FILENO) {
		//redirect output
		if (dup2(outFile, STDOUT_FILENO) == -1) {
			errorMsg();
			return;
		} 	
		if (dup2(outFile, STDERR_FILENO) == -1) {
			errorMsg();
			return;
		}

		fclose(in);
	}
}
  
int searchPath (char *paths[], char execvPath[], char *command) {
	int i = 0;
	
	while (paths[i] != NULL) {
		snprintf(execvPath, 512, "%s/%s", paths[i], command);
		if (access(execvPath, X_OK) == 0) {
			return 0;
		}
		i++;
	}	
	return -1;
}


int main(int argc, char *argv[]) {		
	char *line = NULL;
	size_t len = 0;		
	char *myargv[20];
	char *paths[500]; 
	FILE *fp = stdin;
	ssize_t nread;
	int statval;
	FILE *out = stdout;

	paths[0] = strdup("/bin");
	paths[1] = NULL;

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
			continue;
		}
		
		if (strcmp(myargv[0], "path") == 0) {
			paths[0] = NULL;
			int i = 0;
			for (; i < lenArray -1; i++) {
				paths[i] = strdup(myargv[i+1]);
			}
			
			paths[i+1] = NULL;
			continue;
		}

		int rc = fork();

		if (rc == -1) {
			errorMsg();
		} else if (rc == 0) {
			char execvPath[512];
			int n = searchPath(paths, execvPath, myargv[0]);
			if (n == -1) { 
				errorMsg();
				exit(0);
			}
				
			redirectChild(out);
			
			execvp(execvPath, myargv);
			errorMsg();
			exit(0);

		} else {
			wait(&statval);
			free(line);
			line = NULL;
		}
	}
	free(line);
	fclose(fp);
	return 0;
}
