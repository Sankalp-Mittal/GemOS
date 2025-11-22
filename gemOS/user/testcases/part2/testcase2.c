#include<ulib.h>

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) 
{
	register long a = 1000, b = 100;
        a = (a - b*10);

	if(config_hack_semantics(DIV_ZERO_SKIP, NULL) < 0){
		printf("testcase failed\n");
		exit(0);
	}

	a = b/a;
        printf("skipped a = %d\n", a);
	return 0;
}
