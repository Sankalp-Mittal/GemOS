#ifndef __SIGEXIT_H_
#define __SIGEXIT_H_

#include <types.h>
#include <context.h>

extern long do_sleep(u32 ticks); 
extern long invoke_sync_signal(int signo, u64 *ustackp, u64 *urip); 
extern void do_exit(u8);
extern long do_signal(int signo, unsigned long handler); 
extern long do_alarm(u32 ticks); 
extern int handle_div_by_zero(struct user_regs *regs);
extern int handle_page_fault(struct user_regs *regs);

#endif
