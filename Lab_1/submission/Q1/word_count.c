#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>

// Use Library function such as fopen, geline, fread etc
// use manpages as mentioned in the writeup. 
// -----------------------
//
//
// Your solution begins here
int main(int argc, char *argv[]) {
	//char *argptr = argv[0];
	//printf("%s", argv[0]);
	//printf("%s", argv[1]);
	//printf("%s", argv[2]);
	
	/*while(argptr){
		printf("Arg is: %s\n", argptr);
		argptr++;
	}
	return 0;*/
	char *mode = argv[1];
	//printf("%s", mode);
	char *filepath = argv[2];
	FILE *file_stream = fopen(filepath, "r");
	int characters = 0;
	int new_lines = 0;
	int words = 0;
	//char curr_char = *file_stream;
	//while(curr_char !='\0'){
	//	characters++;
	//	if(curr_char == '\n'){
	//		new_lines++;
	//	}
	//	if(isspace(curr_char)){
	//		words++;
	//	}
	//	file_stream++;
	//	curr_char = *file_stream;
	//}

	
	char *line = NULL;
        size_t len = 0;
        ssize_t nread;	
	
	while ((nread = getline(&line, &len, file_stream)) != -1) {
               //printf("Retrieved line of length %zd:\n", nread);
               //fwrite(line, nread, 1, stdout);
	       new_lines++;
	       characters += nread;
	       if (nread == 1) {
		       continue;
		}
	       int flag = 1;
	       while(*line){
		       if (*line == "\n") {
			       break;
			}
		       while (flag == 1 && isspace(*line)) {
		       		line++;	
			       continue;
				
		       }
		       flag = 0;
		       if(isspace(*line) && !isspace(*(line+1))){
			       words++;
		       }
		       line++;
	       }
	       line = NULL;

        }
	//free(line);
	//fclose(file_stream);

	if(strcmp(mode,"-c") == 0){
		printf("%d\n", characters);
	}
	else if(strcmp(mode, "-l")==0){
		printf("%d\n", new_lines);
	}
	
	else if(strcmp(mode, "-w")==0) {
		printf("%d\n", words);
	}
	else{
		exit(1);
	}
	

	return 0;
}

