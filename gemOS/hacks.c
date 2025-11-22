#include<hacks.h>
#include<sigexit.h>
#include<entry.h>
#include<lib.h>
#include<context.h>
#include<memory.h>

struct hack_config{
	             long cur_hack_config;
		     u64 usr_handler_addr;
};

//Used to store the current hack configuration
static struct hack_config hconfig = {-1, -1};


//system call handler to configure the hack sematics
//user space connection is already created, You need to
//store the hack semantics in the 'hconfig' structure (after validation)
//which will be used when division-by-zero occurs

long sys_config_hs(struct exec_context *ctx, long hack_mode, void *uhaddr)
{
    return 0; 
}


/*This is the handler for division by zero
 * 'regs' is a structure defined in include/context.h which
 * is already filled with the user execution state (by the asm handler)
 * and will be restored back when the function returns 
 *
 */
int do_div_by_zero(struct user_regs *regs)
{	
    return 0;   	
}
