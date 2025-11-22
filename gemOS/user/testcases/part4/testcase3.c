#include<ulib.h>

int func_two(int arg)
{
     int x = printf("%s start\n", __func__);	
     x = x/(x-arg);
     printf("%s end\n", __func__);	
     return x;
}

int func_one(int arg)
{
     int x = printf("%s start\n", __func__);	
     x += func_two(x);	
     printf("%s end x=%d arg=%d\n", __func__, x, arg);	
     return x;
}

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) 
{

	if(config_hack_semantics(DIV_ZERO_SKIP_FUNC, NULL) < 0){
		printf("testcase failed\n");
		exit(0);
	}
        int x = printf("%s start\n", __func__);	
        x += func_one(x);
        printf("%s end x=%d\n", __func__, x);	
	return 0;
}
