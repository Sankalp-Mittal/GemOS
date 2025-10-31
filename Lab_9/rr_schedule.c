#include<context.h>
#include<page.h>
#include<memory.h>
#include<lib.h>


//////////////////////  Q1: RR Scheduling   ///////////////////////////////////////
//args:
//      ctx: new exec_context to be added in the linked list
void rr_add_context(struct exec_context *ctx)
{
      /*TODO*/
	//printk("add %x\n", ctx->pid);
	struct exec_context* curr_node = rr_list_head;
	if(ctx->pid == 0x0) return;
	if(curr_node == NULL){
		rr_list_head = ctx;
		ctx->next = NULL;
		return;
	}
	while(curr_node->next!= NULL) {
		//printk("Ye waala %x\n", curr_node->pid);
		curr_node = curr_node -> next;
	}
	curr_node -> next = ctx;
	ctx->next = NULL;
       return;
}

//args:
//      ctx: exec_context to be removed from the linked list
void rr_remove_context(struct exec_context *ctx)
{
      /*TODO*/
	if(ctx->pid == 0x0){
		return;
	}
	//printk("remove %x\n", ctx->pid);
	struct exec_context* curr_node = rr_list_head;
	if (curr_node == NULL) {
		return;
	}
	if (curr_node -> next == NULL)  {
		rr_list_head = NULL;
		return;
	}

	if(curr_node->pid == ctx->pid){
		rr_list_head = curr_node -> next;
		return;
	}

	struct exec_context* prev = curr_node;
	curr_node = curr_node -> next;
	while (curr_node->pid != ctx->pid && curr_node != NULL){
		//printk("yoyo %x\n", curr_node->pid);
		prev = curr_node;
		curr_node = curr_node -> next;
	}
	prev -> next = curr_node -> next;
	//ctx->next = NULL;
      return;
}

//args:
//      ctx: exec_context corresponding the currently running process
struct exec_context *rr_pick_next_context(struct exec_context *ctx)
{
    /*TODO*/
	//printk("pick next %x\n", ctx->pid);
	// int q = 0;
	if(ctx->pid == 0x0){
		//printk("Swapper\n");
		if(rr_list_head == NULL) return ctx;
		return rr_list_head;
	}
	struct exec_context* curr_node = rr_list_head;
	if(curr_node == NULL) return get_ctx_by_pid(0);
	if( curr_node -> pid == ctx->pid && curr_node->next == NULL) return ctx;
	//if( curr_node == NULL) return get_ctx_by_pid(0);
	
	if(ctx -> next == NULL) return rr_list_head;

	return ctx->next;
     //return get_ctx_by_pid(0);
}

//////////////////////  Q2: Get the PAGE TABLE details for given address   ///////////////////////////////////////


//args:
//      ctx: exec_context corresponding the currently running process
//      addr: address for which the PAGE TABLE details are to be printed

int do_walk_pt(struct exec_context *ctx, unsigned long addr)
{
    u64 *vaddr_base = (u64 *)osmap(ctx->pgd);
    u64 arr[4];
    //printk("addr:%x\n",addr);
    arr[0] = (addr & PGD_MASK) >> PGD_SHIFT;
    arr[1] = (addr & PUD_MASK) >> PUD_SHIFT;
    arr[2] = (addr & PMD_MASK) >> PMD_SHIFT;
    arr[3] = (addr & PTE_MASK) >> PTE_SHIFT;
    int isValid = 1;
    for(int i=1; i<=4; i++){
	//printk("arr[%d]:%x\n",i,arr[i-1]);
    	u64 ofset = arr[i-1];
		u64 pg_addr = vaddr_base+ofset;
		u64 contents = *((u64*)pg_addr);
		if((contents & 0x1) && isValid){
			u64 physical = (contents & (0xFFFF000)) >> 12;
			u64 tags = (contents & 0xFFF);
			printk("L%d-entry addr: %x, L%d-entry contents: %x, PFN: %x, Flags: %x\n", i, pg_addr, i, contents, physical, tags);
			vaddr_base = (physical << 12);
		}
		else{
			printk("No L%d entry\n",i);
			isValid = 0;
		}

    }

    /*TODO*/

    return 0;
}

