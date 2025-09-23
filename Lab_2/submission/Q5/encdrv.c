#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<assert.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_CHARS_IN_LINE 1000

int main(int argc, char **argv)
{
	char buf[MAX_CHARS_IN_LINE+1];
	char obuf[MAX_CHARS_IN_LINE+1];

	// NOTE: Do not modify anything above this line	
	/***
	 *      Your Code goes here
	 */	
	// AHH	
	int writefd[2];
	int readfd[2];
	
	pipe (writefd);
	
	pid_t cpid;

	const char * inputFile = argv[1];
	const char * outputFile = argv[2];

	int inputfd = open(inputFile, O_RDONLY);
	int outputfd = open(outputFile, O_CREAT | O_WRONLY, 0666);
	assert(outputfd > 0);

	char * line = NULL;
	size_t len = 0;
	
	// while ((len = read(inputfd, buf, MAX_CHARS_IN_LINE + 1)) != 0) {
		
	cpid = fork();
	if (cpid == 0) {
		// Child
		// printf("1\n");	
		close(1);
		// printf("2\n");	
		dup(outputfd);
		// printf("3\n");	
		close(0);
		// printf("4\n");	
		dup(writefd[0]);	
		// printf("5\n");	
		close(writefd[1]);
		// printf("6\n");	
		char linelen[32];
		// printf("7\n");	
		sprintf(linelen, "%ld\n", strlen(buf));
		// printf("8\n");	
		
		execl("./encrypt", "encrypt", NULL);
		// printf("9\n");	

	}

	else {
		// Parent	
		close (writefd[0]);

		int ret = 0;
		while((ret = read(inputfd, buf, MAX_CHARS_IN_LINE)) != 0) {
			// obuf[ret] = ' ';
			// printf("parent 1\n");
			
			char line[16];


			if (ret == MAX_CHARS_IN_LINE) {
				sprintf (line, "%d\n", ret);
			}
			else {
				// ret--;
				sprintf (line, "%d\n", ret);
			}
			// printf("parent 2\n");
			
			write (writefd[1], line, strlen(line));
			// printf("parent 3\n");
			
			write (writefd[1], buf, ret);

		}

	}

	// }
	// write (writefd[1], "1\n", 2);
	// write (writefd[1], "\n", 1);
	// printf("parent F");
	close(inputfd);
	close(outputfd);

	return 0;
}

