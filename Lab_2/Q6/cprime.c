#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<sys/time.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/syscall.h>

extern int isPrime(long x); // Returns zero if composite.
                            // Returns one if prime. 

long find_primes(int num_cp, long M)
{
   /*
    *  TODO 
    *  Your code goes here
    */
   
   int Arr_pipefd[num_cp][2];
   pid_t cpid;
   
   for (int i = 0; i < num_cp; i++) {
   	syscall (SYS_pipe, Arr_pipefd[i]);
   } 

   long curr = 1;
   long rrange = M / num_cp;
   long left = M - rrange * num_cp;

   pid_t *Process_Arr = (pid_t *) calloc (num_cp, sizeof(pid_t));

   for (int i = 0; i < num_cp; i++) {
	int child_proc_id = i;
   	cpid = fork();
	Process_Arr [i] = cpid;
	if (cpid == -1) {
		perror("fork");
		exit (-1);
	}

	else if (cpid == 0) { 		// Child
		close (Arr_pipefd[i][0]);	// Read End
		long count = 0;
		long end = curr + rrange - 1;
		if (i < left) {
			end++;
		}
		printf("Child id %d start\n", child_proc_id);


		// printf("Curr End count %ld, %ld, %ld\n", curr, end, count);
		for (long q = curr; q <= end; q++) {
			if (isPrime (q)) {
				count++;
			}
		}

		printf("Curr End count %ld, %ld, %ld\n", curr, end, count);
		
		char str[64];

		sprintf(str, "%ld", count);

		// Communicate Answer to parent
		//

		printf("STR %s\n", str);

		printf("Number of bytes: %ld\n", strlen(str));
		write (Arr_pipefd[i][1], str, strlen(str));
		

		close (Arr_pipefd[i][1]);
		// printf("Child id %d Exiting\n", child_proc_id);
		exit (0);
	}

	else {
		curr += rrange;
		if (i < left) {
			curr++;
		}
	}

   }
   
   for (int i = 0; i < num_cp; i++) {
	int wstatus;
	// printf("Waiting\n");
	waitpid(Process_Arr[i], &wstatus, 0);
   }

   long Answer = 0;

   for (int i = 0; i < num_cp; i++) {
	close(Arr_pipefd[i][1]);
	char buf[64];

	int ret = read(Arr_pipefd[i][0], buf, 64);
	dprintf(2, "read returned %d\n", ret);
	buf[ret] = ' '; 
	printf("Buf = %s\n", buf);
	
	Answer += atol (buf);
	
	close(Arr_pipefd[i][0]);
   }

   return Answer;   
}
