#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char **argv) {

    /**
     * TODO: Write your code here.
     */
    if(argc != 3){
	    printf("Error\n");
	    exit(-1);
    }
    const char* search_term = argv[1];
    const char* file_path = argv[2];
    int len_search = strlen(search_term);

    int fd = open (file_path,O_RDONLY);
    if(fd<0){
	    printf("Error\n");
	    exit(-1);
    }
    char buf[256];
    int isFound = 0;
    while(1){
	ssize_t read_bytes = read(fd,buf,len_search);
    	if(read_bytes<0){
		printf("Error\n");
		exit(-1);
    	}
	if(read_bytes < len_search) break;

	if(strcmp(search_term,buf)==0){
		isFound=1;
		break;
	}
	lseek(fd, 1-len_search, SEEK_CUR);
	
    }
    if(isFound){
	    printf("FOUND\n");
    }
    else{
	    printf("NOT FOUND\n");
    }
    close(fd);


    return 0;
}
