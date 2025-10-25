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
    
	if(hack_mode == DIV_ZERO_USH_EXIT){
		if((u64)uhaddr < CODE_START || (u64)uhaddr > RODATA_START){
			printk("Error...exiting\n");
			do_exit(0);
		}
	}
	if(hack_mode>5){
		printk("Error...exiting\n");
		do_exit(0);
	}
	hconfig.cur_hack_config = hack_mode; 
	hconfig.usr_handler_addr = (u64)uhaddr;
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

	if(hconfig.cur_hack_config == DIV_ZERO_OPER_CHANGE){
		u64 temp = regs->rax;
		regs->rax = regs->rcx;
		regs->rcx = temp;
		//printk("rcx:%d, rax:%d", regs->rax, regs->rcx);
		//do_exit(0);
	}
	if(hconfig.cur_hack_config == DIV_ZERO_SKIP){
		regs->entry_rip += 3;
	}
	if(hconfig.cur_hack_config == DIV_ZERO_USH_EXIT){
		regs->rdi = regs->entry_rip;
		regs->entry_rip = hconfig.usr_handler_addr;
	}
	if(hconfig.cur_hack_config == DIV_ZERO_SKIP_FUNC){
		regs->entry_rsp = regs->rbp;
		regs->rbp = *((u64*)regs->entry_rsp);
		regs->entry_rsp += 8;
		regs->entry_rip = *((u64*)regs->entry_rsp);
		regs->entry_rsp += 8;
		regs->rax = 1;

	}	
    return 0;   	
}
