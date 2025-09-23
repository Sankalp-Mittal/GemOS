#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<sys/mman.h>

long how_large(void *address)
{
  long temp = (long)address;
  if(temp%4096){
	  temp -= temp%4096;
  }
  
  long CONST = 4096;

  void* temp1 = mmap((void*)temp,4096,PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
  
	  munmap((void*)temp1,4096);
  if((long)temp1==temp){
	 // munmap((void*)temp,4096);
	  return 0;
  }
  long left = temp;
  long temp2 = temp;
  while (1){
	  //long temp2 = temp;
	  temp2 -= CONST;
	  
  	  void* temp3 = mmap((void*)temp2,4096,PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
          //printf("");
	  munmap((void*)temp3,4096);
  	  if((long)temp3==temp2){
	 	// munmap((void*)temp,4096);
	  	//return 0;
		left = temp2 + CONST;
		
		break;
  	  }

  }

  long right = temp + CONST;
  temp2 = temp;
  while (1){
	  //long temp2 = temp;
	  temp2 += CONST;

  	  void* temp3 = mmap((void*)temp2,4096,PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
  
	  munmap((void*)temp3,4096);
  	  if((long)temp3==temp2){
	 	// munmap((void*)temp,4096);
	  	//return 0;
		right = temp2;
		break;
  	  }

  }
  return right - left;    
}
