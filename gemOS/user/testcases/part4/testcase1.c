#include<ulib.h>

void func_two()
{
     int x = printf("%s start\n", __func__);	
     x = x/(x-15);
     printf("%s end\n", __func__);	
}

void func_one()
{
     printf("%s start\n", __func__);	
     func_two();	
     printf("%s end\n", __func__);	
}

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) 
{
	if(config_hack_semantics(DIV_ZERO_SKIP_FUNC, NULL) < 0){
		printf("testcase failed\n");
		exit(0);
	}
        printf("%s start\n", __func__);	
        func_one();
        printf("%s end\n", __func__);	
	return 0;
}
