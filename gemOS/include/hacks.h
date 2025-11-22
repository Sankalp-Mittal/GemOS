#ifndef __HACKS_H_
#define __HACKS_H_

#include <types.h>
#include <context.h>

enum{
	DIV_ZERO_OPER_CHANGE,
	DIV_ZERO_SKIP,
	DIV_ZERO_USH_EXIT,
	DIV_ZERO_SKIP_FUNC,
	DIV_ZERO_MAX
};

extern long sys_config_hs(struct exec_context *ctx, long hack_mode, void *uhaddr);

#endif
