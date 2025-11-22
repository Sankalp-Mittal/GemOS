#include<ulib.h>

static long fact(long num){
   
    long myfact;   	
    if(num == 1)
         return 1;
    else{  
	long fact_one_less = fact(num-1);
        printf("Invocation %d fact_one_less %d\n",num, fact_one_less); 	
        if(num % 2)
	     num = num / ((num & 0x1) -1);       
	 return num * fact_one_less;     
    }
}

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) 
{

	if(config_hack_semantics(DIV_ZERO_SKIP_FUNC, NULL) < 0){
		printf("testcase failed\n");
		exit(0);
	}
	printf("final %d\n", fact(5));
	return 0;
}
