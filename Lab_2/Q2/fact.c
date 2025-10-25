#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<sys/time.h>
#include<sys/wait.h>


int main(int argc, char **argv)
{
   /*Your code goes here */
   //printf("%d\n", argc);
   int input = atoi(argv[1]);
   //printf("%i\n", input);
   int curr_p = 1;

   if(argc == 3){
	curr_p = atoi(argv[2]);

   	//printf("%i\n", curr_p);
   }
   curr_p *= input;
   input--;
   if(input==1){
	  printf("%d\n", curr_p);
	  exit(0);
   }
   else{
	   char* str_input = (char*) malloc ( 8 * sizeof(int) );
	   sprintf(str_input,"%d",input);
	   //printf("%s\n",str_input);
	   char* str_curr_p = (char*) malloc ( 8 * sizeof(int) );

	   sprintf(str_curr_p, "%d",curr_p);
	   execl("./fact", "fact", str_input, str_curr_p, NULL );
   }

   return 0;
}
