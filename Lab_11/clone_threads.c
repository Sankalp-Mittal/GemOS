#include<clone_threads.h>
#include<entry.h>
#include<context.h>
#include<memory.h>
#include<lib.h>
#include<mmap.h>
#include<fork.h>
#include<page.h>

extern int destroy_user_mappings(struct exec_context *ctx);

static void set_kstack_of_thread(struct exec_context *ctx)
{
   ctx->os_stack_pfn = os_pfn_alloc(OS_PT_REG);
   ctx->os_rsp = (((u64) ctx->os_stack_pfn) << PAGE_SHIFT) + PAGE_SIZE;
   stats->num_processes++;
   ctx->type = EXEC_CTX_USER_TH;	
}

//XXX Do not modify anything above this line

/*
  system call handler for clone, create thread like execution contexts
*/
long do_clone(void *th_func, void *user_stack, void *user_arg) 
{
  int ctr;
  struct exec_context *new_ctx = get_new_ctx();  //This is to be used for the newly created thread
  struct exec_context *ctx = get_current_ctx();
  u32 pid = new_ctx->pid;
  struct thread *n_thread;

  if(!ctx->ctx_threads){  // This is the first thread
          ctx->ctx_threads = os_alloc(sizeof(struct ctx_thread_info));
          bzero((char *)ctx->ctx_threads, sizeof(struct ctx_thread_info));
          ctx->ctx_threads->pid = ctx->pid;
  }

 /* XXX Do not modify anything above. Your implementation goes here */

 // TODO your code goes here
 n_thread = os_alloc(sizeof(struct thread));
 n_thread->pid = pid;
 n_thread->status = TH_USED;
 n_thread->parent_ctx = ctx;

 int isSuccess = 0;
 for(int i=0; i<MAX_THREADS; i++){
	if(ctx->ctx_threads->threads[i].status == TH_UNUSED){
		memcpy((void*)(ctx->ctx_threads->threads + i), n_thread, sizeof(struct thread));
		isSuccess = 1;
		break;
	}

 }

 os_free( n_thread, sizeof(struct thread));

 if(!isSuccess) return -1;
 memcpy(new_ctx, ctx, sizeof(struct exec_context));
 new_ctx->ctx_threads = NULL;
 new_ctx->pid = pid;
 new_ctx->ppid = ctx->pid; 
 new_ctx->type = EXEC_CTX_USER_TH;
 new_ctx->state = READY;

 //bzero((char *)&(new_ctx->regs), sizeof(struct user_regs));
 //u64* new_stack_ptr = (u64*)user_stack;
 //*new_stack_ptr = ctx->regs.rbp;

 new_ctx->regs.entry_rip = (u64) th_func;
 new_ctx->regs.rbp = (u64) user_stack;
 new_ctx->regs.entry_rsp = (u64) user_stack;
 new_ctx->regs.rdi = (u64) user_arg;

 //End of your logic
  
 //XXX The following two lines should be there. 
  //printk("hi he ha\n"); 
  set_kstack_of_thread(new_ctx);  //Allocate kstack for the thread
  return pid;
}



//handler for exit()
void do_exit(u8 normal)
{
  int ctr;
  struct exec_context *ctx = get_current_ctx();
  struct exec_context *new_ctx;

  if(isThread(ctx)){
	  handle_thread_exit(ctx,1);
	  if(!put_pfn(ctx->os_stack_pfn))
    		 os_pfn_free(OS_PT_REG, ctx->os_stack_pfn);
  	  release_context(ctx);
  }
  if(isProcess(ctx)){
	do_file_exit(ctx);   // Cleanup the files

  // cleanup of this process
  	destroy_user_mappings(ctx); 
  	do_vma_exit(ctx);
  	if(!put_pfn(ctx->pgd)) 
      		os_pfn_free(OS_PT_REG, ctx->pgd);   //XXX Now its fine as it is a single core system
  	if(!put_pfn(ctx->os_stack_pfn))
     		os_pfn_free(OS_PT_REG, ctx->os_stack_pfn);
	cleanup_all_threads(ctx);  
	release_context(ctx); 
  }

  new_ctx = pick_next_context(ctx);
  //printk("Scheduling %s:%d [ptr = %x]\n", new_ctx->name, new_ctx->pid, new_ctx);	
  schedule(new_ctx);  //Calling from exit
  //:printk("6\n");
}

// XXX Reference implementation for a process exit
// You can refer this to implement your version of do_exit
/*
void do_exit(u8 normal) 
{
  int ctr;
  struct exec_context *ctx = get_current_ctx();
  struct exec_context *new_ctx;

 
  do_file_exit(ctx);   // Cleanup the files

  // cleanup of this process
  destroy_user_mappings(ctx); 
  do_vma_exit(ctx);
  if(!put_pfn(ctx->pgd)) 
      os_pfn_free(OS_PT_REG, ctx->pgd);   //XXX Now its fine as it is a single core system
  if(!put_pfn(ctx->os_stack_pfn))
     os_pfn_free(OS_PT_REG, ctx->os_stack_pfn);
  release_context(ctx); 
  new_ct
  x = pick_next_context(ctx);
  dprintk("Scheduling %s:%d [ptr = %x]\n", new_ctx->name, new_ctx->pid, new_ctx); 
  schedule(new_ctx);  //Calling from exit
}
*/


////////////////////////////////////////////////////////// Semaphore implementation ////////////////////////////////////////////////////
//
//


// A spin lock implementation using cmpxchg
// XXX you can use it for implementing the semaphore
// Do not modify this code

static void spin_init(struct spinlock *spinlock)
{
	spinlock->value = 0;
	//printk("spinlock initialised\n");
}

static void spin_lock(struct spinlock *spinlock)
{
	unsigned long *addr = &(spinlock->value);

	asm volatile(
		"mov $1,  %%rcx;"
		"mov %0,  %%rdi;"
		"try: xor %%rax, %%rax;"
		"lock cmpxchg %%rcx, (%%rdi);"
		"jnz try;"
		:
		: "r"(addr)
		: "rcx", "rdi", "rax", "memory"
	);
}

static void spin_unlock(struct spinlock *spinlock)
{
	spinlock->value = 0;
}

static int init_sem_metadata_in_context(struct exec_context *ctx)
{
   if(ctx->lock){
	   printk("Already initialized MD. Call only for the first time\n");
	   return -1;
   }
   ctx->lock = (struct lock*) os_alloc(sizeof(struct lock) * MAX_LOCKS);
   if(ctx->lock == NULL){
			printk("[pid: %d]BUG: Out of memory!\n", ctx->pid);
                        return -1;
   }
	
   for(int i=0; i<MAX_LOCKS; i++)
			ctx->lock[i].state = LOCK_UNUSED;
}

// XXX Do not modify anything above this line

/*
  system call handler for semaphore creation
*/
int do_sem_init(struct exec_context *current, sem_t *sem_id, int value)
{
	if(current->lock == NULL)
		init_sem_metadata_in_context(current);
        // TODO Your implementation goes here
	int isSuccess = 0;
	struct semaphore *semt = os_alloc(sizeof(struct semaphore));
        semt->value = value;
        spin_init(&(semt->lock));
	semt->wait_queue = NULL;
	//printk("Sem id: %x\n", (u64)sem_id);

	for(int i=0; i<MAX_LOCKS; i++){
		if(current->lock[i].state == LOCK_UNUSED){
			current->lock[i].state = LOCK_USED;
			current->lock[i].id = (u64) sem_id;
			//*sem_id = current->lock[i].id;
			//struct semaphore *semt = os_alloc(sizeof(struct semaphore));
			//semt->value = value;
			//spin_init(&(semt->lock));
			
			current->lock[i].sem = *semt;
			isSuccess = 1;
			break;
		}
	}

	if(!isSuccess) return -EAGAIN;

	return 0;

	//return -1;
}

/*
  system call handler for semaphore acquire
*/

int do_sem_wait(struct exec_context *current, sem_t *sem_id)
{
	for(int i=0;i<MAX_LOCKS; i++){
		if(current->lock[i].id == (u64)sem_id && current->lock[i].state == LOCK_USED){
			//printk("Starting Wait: %x\n", (u64)sem_id);
			//printk("Id: %d\n", i);
			//spin_lock(&(current->lock[i].sem.lock));
			if(current->lock[i].sem.value > 0){
				spin_lock(&(current->lock[i].sem.lock));
				//printk("Value %d\n", current->lock[i].sem.value);
				current->lock[i].sem.value--;
				spin_unlock(&(current->lock[i].sem.lock));
			}
			else{
				spin_lock(&(current->lock[i].sem.lock));
				struct exec_context* curr = current->lock[i].sem.wait_queue;
				struct exec_context* next_proc = pick_next_context(current);
				if(curr == NULL){
					current->lock[i].sem.wait_queue = current;
				}
				else{
					while(curr->next != NULL){
						curr = curr->next;
					}

					curr->next = current;
				}
				current->next = NULL;
				current->state = WAITING;
				spin_unlock(&(current->lock[i].sem.lock));
				schedule(next_proc);
			}
			//spin_unlock(&(current->lock[i].sem.lock));
		}
	}
	return 0;
}

/*
  system call handler for semaphore release
*/
int do_sem_post(struct exec_context *current, sem_t *sem_id)
{
	//printk("Post Sem: %x\n", (u64)sem_id);	
	for(int i=0;i<MAX_LOCKS; i++){
		if(current->lock[i].id == (u64)sem_id && current->lock[i].state == LOCK_USED){
			//printk("id: %d\n", i);
			spin_lock(&(current->lock[i].sem.lock));
			current->lock[i].sem.value++;
			spin_unlock(&(current->lock[i].sem.lock));
			if(current->lock[i].sem.wait_queue == NULL) break;
			else{
				spin_lock(&(current->lock[i].sem.lock));
				struct exec_context* proc = current->lock[i].sem.wait_queue;
				struct exec_context* temp = proc;
				while(temp != NULL){
					//printk("PID %x\n", temp->pid);
					temp = temp->next;
				}
				current->lock[i].sem.wait_queue = proc->next;
				proc->next = NULL;
				current->lock[i].sem.value--;
				proc->state = READY;
				//printk("Wake up: %x\n", proc->pid);
				spin_unlock(&(current->lock[i].sem.lock));
				//schedule(proc);
			}
		}
	}
	return 0;
}
