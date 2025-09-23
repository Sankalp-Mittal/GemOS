#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

int main(int argc, char **argv)
{
    /**
     * TODO: Write your code here.
     */
   const char* file_path = argv[1];
   int fd = open (file_path,O_RDONLY);
    if(fd<0){
	    printf("Error\n");
	    exit(-1);
    }
    char buf[256];
    
    int num_openat=0,num_close=0,num_read=0,num_write=0,num_stat=0,num_execve=0;
    
    char* search_term = " openat(";
    buf[8] = '\0';
    int len_search = strlen(search_term);
    
    while(1){
	ssize_t read_bytes = read(fd,buf,len_search);
    	if(read_bytes<0){
		printf("Error\n");
		exit(-1);
    	}
	if(read_bytes < len_search) break;

	if(strcmp(search_term,buf)==0){
		num_openat++;
	}
	lseek(fd, 1-len_search, SEEK_CUR);	
    }
    
    //close(fd);
    
    off_t of2 = lseek(fd, 0, SEEK_END);
    lseek(fd,-of2,SEEK_END);
   // printf("%ld Diff=%d",of,st);
    //fd = open(file_path,O_RDONLY);
    
    char* close_str = " close(";
    //search_term = "close";
    len_search = strlen(close_str);
    //printf("Len search: %d\n",len_search);
    
    buf[7] = '\0';    
    while(1){
	ssize_t read_bytes = read(fd,buf,len_search);
    	// printf("%s Diff:%d, btyes:%ld\n",buf,strcmp(close_str,buf),read_bytes);
	if(read_bytes<0){
		printf("Error\n");
		exit(-1);
    	}
	if(read_bytes < len_search) break;
	//printf("Lo\n");
	if(strcmp(close_str,buf)==0){
		num_close++;
	//	printf("Yo\n");
	}

	lseek(fd, 1-len_search, SEEK_CUR);	
    }
    // lseek(fd, 0, SEEK_SET);
    off_t of3 = lseek(fd, 0, SEEK_END);
    lseek(fd,-of3,SEEK_END);
    
    buf[6] = '\0';
    char* read_str = " read(";
    len_search = strlen(read_str);
    while(1){
	ssize_t read_bytes = read(fd,buf,len_search);
    	if(read_bytes<0){
		printf("Error\n");
		exit(-1);
    	}
	if(read_bytes < len_search) break;

	if(strcmp(read_str,buf)==0){
		num_read++;
	}
	lseek(fd, 1-len_search, SEEK_CUR);	
    }
    // lseek(fd, 0, SEEK_SET);
    off_t of4 = lseek(fd, 0, SEEK_END);
    lseek(fd,-of4,SEEK_END);
    char* write_str = " write(";
    buf[7] = '\0';
    len_search = strlen(write_str);
    while(1){
	ssize_t read_bytes = read(fd,buf,len_search);
    	if(read_bytes<0){
		printf("Error\n");
		exit(-1);
    	}
	if(read_bytes < len_search) break;

	if(strcmp(write_str,buf)==0){
		num_write++;
	}
	lseek(fd, 1-len_search, SEEK_CUR);	
    }
    // lseek(fd, 0, SEEK_SET);
    off_t of5 = lseek(fd, 0, SEEK_END);
    lseek(fd,-of5,SEEK_END);
    char* stat_str = " stat(";
    len_search = strlen(stat_str);
    buf[6] = '\0';
    while(1){
	ssize_t read_bytes = read(fd,buf,len_search);
    	if(read_bytes<0){
		printf("Error\n");
		exit(-1);
    	}
	if(read_bytes < len_search) break;

	if(strcmp(stat_str,buf)==0){
		num_stat++;
	}
	lseek(fd, 1-len_search, SEEK_CUR);	
    }
    // lseek(fd, 0, SEEK_SET);
    off_t of6 = lseek(fd, 0, SEEK_END);
    lseek(fd,-of6,SEEK_END);
    buf[0] = ' ';
    buf[1] = ' ';
    buf[2] = ' ';
    buf[3] = ' ';
    buf[4] = ' ';
    buf[5] = ' ';
    buf[6] = ' ';
    buf[7] = ' ';
    buf[8] = '\0';
    char* exec_str = " execve(";
    len_search = strlen(exec_str);
    while(1){
	ssize_t read_bytes = read(fd,buf,len_search);
    	if(read_bytes<0){
		printf("Error\n");
		exit(-1);
    	}
	if(read_bytes < len_search) break;

	if(strcmp(exec_str,buf)==0){
		num_execve++;
	}
	lseek(fd, 1-len_search, SEEK_CUR);	
    }
   printf("openat: %d\n",num_openat);
   printf("close: %d\n",num_close);
   printf("read: %d\n",num_read);
   printf("write: %d\n",num_write);
   printf("stat: %d\n",num_stat);
   printf("execve: %d\n",num_execve);
  return 0;
}
