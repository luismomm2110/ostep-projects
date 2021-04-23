#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE (512)

int main (int argc, char *argv[]) {
	
	if (argc == 1) {
		printf("wzip: file1 [file2 ...]\n");
		exit(1);
	}
	
	int i;
	for (i=1; i < argc; i++) {
		FILE *fp = fopen(argv[i], "r"); 
		 if (fp == NULL) {
			     printf("wzip: cannot open file\n");
			         exit(1);
		 }

		char comparison[2];
	       	int repetions = 1;

		comparison[0] = fgetc(fp);

		while((comparison[1] = fgetc(fp)) != EOF) {
			
			if (comparison[1] == comparison[0]) {
				repetions++;
			} else {
				fwrite(&repetions, sizeof(int), 1, stdout);
				fwrite(&comparison[0], 1, 1, stdout);
				comparison[0] = comparison[1];
				repetions = 1;
			}		
		}
		
		fclose(fp);
	}	
	return 0;
}
