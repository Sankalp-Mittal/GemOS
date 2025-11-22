#include<ulib.h>

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) 
{
	long a = printf("Division by zero\n");
        long result;
	
	if(config_hack_semantics(DIV_ZERO_OPER_CHANGE, NULL) < 0){
		printf("testcase failed\n");
		exit(0);
	}

        result = a/(a-17);
        printf("result %d\n", result);
	return 0;
}
