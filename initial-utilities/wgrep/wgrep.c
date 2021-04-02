//this file is for the OS course 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main (int argc, char *argv[]) {
	
	if (argc == 1) {
	  printf("wgrep: searchterm [file ...]\n");
	  exit(1); 
	}
	
	size_t size = 512;
	char *buffer;
	buffer = (char *) malloc(size);
	int i;
	for (i=2; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r");
		if (fp==NULL) {
	        	printf("wgrep: cannot open file\n");
			exit(1);
		} 
			
   		while(getline(&buffer,&size, fp) != -1) {
			if (strstr(buffer,argv[1]) != NULL) {
				printf("%s", buffer);
		        }
	        }

	fclose(fp);

	}

	if (argc == 2) {
		while(getline(&buffer, &size, stdin) != -1) {
		     if (strstr(buffer,argv[1]) != NULL) {
			printf("%s", buffer);
		     }

		}	
	}
	
	return 0;
}
