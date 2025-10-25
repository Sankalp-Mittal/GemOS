#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<sys/time.h>
#include<sys/wait.h>


extern int isPrime(long x); // Returns zero if composite.
                            // Returns one if prime. You may use this if you want. 
int find_primes(long M)
{
   long limit = M/2;
   pid_t pid1 = fork();
   if(!pid1){
	   long num_primes = 0;
	   for(long i=2;i<=limit;i++){
		   if(isPrime(i)) num_primes++;
	   }
	   exit((int)(num_primes%256));
   }

   pid_t pid2 = fork();

   if(!pid2){
	   long num_primes = 0;
	   for(long i=limit+1;i<=M;i++){
		   if(isPrime(i)) num_primes++;
	   }
	   exit((int)(num_primes%256));
   }
   int total = 0;
   int temp_primes;
   waitpid(pid1, &temp_primes,0);
   total += WEXITSTATUS(temp_primes);
   waitpid(pid2, &temp_primes,0);
   total += WEXITSTATUS(temp_primes);
   return total%256;
}
