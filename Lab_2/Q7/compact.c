#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void compact(void *start, void *end)
{
  /* 
   * TODO
   * Your code goes here
   */
  char *itrf = start;
  char *itrc = start;
  int diff = 0;
  long endi = (long) end;
  while ((long) itrf < endi) {
  	if (*(itrf) == '\0') {
		diff++;
		itrf++;
		continue;
	}
	else {
		*(itrc) = *(itrf);
		itrc++;
		itrf++;
	}
  }
  
  end = sbrk (-1 * diff);  

  return;    
}
