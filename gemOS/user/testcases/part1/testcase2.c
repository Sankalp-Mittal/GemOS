#include<ulib.h>

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) 
{
	unsigned long a = 1000, b = 100;
        a = (a - b*10);

	if(config_hack_semantics(100, NULL) == 0){
		printf("testcase failed\n");
		exit(0);
	}

	a = b/a;
        printf("testcase failed");
	return 0;
}
