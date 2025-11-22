#include<ulib.h>

static void user_handler(u64 bad_code_addr){
    printf("Hurray... the user space handler is called %x\n", bad_code_addr);
    if(config_hack_semantics(DIV_ZERO_SKIP, NULL) < 0){
		printf("testcase failed\n");
		exit(0);
    }
    u64 shifted = bad_code_addr >> 40;
    printf("%x\n", shifted);
    printf("%x\n", bad_code_addr/shifted);
    exit(0);    
}

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) 
{
	register long a = 1000, b = 100;
        a = (a - b*10);

	if(config_hack_semantics(DIV_ZERO_USH_EXIT, &user_handler) < 0){
		printf("testcase failed\n");
		exit(0);
	}

	a = b/a;
        printf("%d testcase failed\n", a);
	return 0;
}
