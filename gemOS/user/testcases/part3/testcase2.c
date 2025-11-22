#include<ulib.h>

static void user_handler(void){
    printf("Hurray... the user space handler is called\n");
    exit(0);    
}

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) 
{
	long a = 1000, b = 100;
        a = (a - b*10);

	if(config_hack_semantics(DIV_ZERO_USH_EXIT, &a) == 0){
		printf("testcase failed\n");
		exit(0);
	}

	a = b/a;
        printf("%d testcase failed\n", a);
	return 0;
}
