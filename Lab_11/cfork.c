#include <fork.h>
#include <page.h>
#include <mmap.h>
#include <apic.h>

#define PAGE_MASK 0xFFFFFFFFFFFFF000
#define PGD_MASK 0xFF8000000000
#define PUD_MASK 0x007FC0000000
#define PMD_MASK 0x00003FE00000
#define PTE_MASK 0x0000001FF000
#define ADO_MASK 0x000000000FFF

#define PGD_SHIFT 39
#define PUD_SHIFT 30
#define PMD_SHIFT 21
#define PTE_SHIFT 12


/* #################################################*/

static inline void invlpg(unsigned long addr) {
    asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}
/**
 * cfork system call implemenations
 */

void create_new_page_table(u64* parent_pte, u64* child_pte, int level){
	printk("level %d\n", level);
	if(level==3){
		
		for (u64 i = 0; i <=0x1FF; i++) {
			//printk("index: %d\n", i);
			u64* child_address = (child_pte + i);
			u64 parent_entry = *(parent_pte + i);
			printk("parent entry %x\n", parent_entry);
			if(parent_entry != 0x0){
				get_pfn(parent_entry & PAGE_MASK);
				parent_entry = parent_entry & 0xFFFFFFFFFFFFFFFB;
				*child_address = parent_entry;
			}
		}
	}
	else {
		for (u64 i = 0; i <=0x1FF; i++) {
			//printk("index: %d\n", i);
			u64* child_address = (child_pte + i);
			u64 parent_entry = *(parent_pte + i);
			printk("index: %d, parent entry %x\n", i, parent_entry);
			if(parent_entry != 0x0){
				u64 new_add = os_pfn_alloc(OS_PT_REG);
				*child_address = (new_add & PAGE_MASK);
				u64 parentFlags = parent_entry & 0xFFF;
				*child_address += parentFlags;
				create_new_page_table((u64*)(parent_entry & PAGE_MASK), (u64*)(new_add), level+1);
			}
		}
		//create_new_page_table(,,level+1);
	}
}

void page_table_walk (u64* parent_pgd, u64* child_pgd, u64 address) {

	u64* vaddr_base = (u64 *) osmap ((u64)parent_pgd);
	u64* vaddr_base_child = (u64 *) osmap ((u64)child_pgd);
	
	u64 arr[4];
	arr[0] = (address & PGD_MASK) >> PGD_SHIFT;
	arr[1] = (address & PUD_MASK) >> PUD_SHIFT;
	arr[2] = (address & PMD_MASK) >> PMD_SHIFT;
	arr[3] = (address & PTE_MASK) >> PTE_SHIFT;
	
	u64 physical = 0x0;
	for(int i=0; i<4; i++){
		//printk("Physical i:%d\n", i);
		u64* curr = (u64*)(vaddr_base + arr[i]);
		u64 entry = *curr;
		if(entry == 0) break;
		if(i==3){
			*curr = *curr & 0xFFFFFFFFFFFFFFFB;
			physical = *curr;
			break;
		}
		u64 newAddr = entry & PAGE_MASK;
		vaddr_base = newAddr;
		//if(i==3) physical = address;
	}

	if(physical == 0x0) return;

	//u64 physical = 0x0;
	for(int i=0; i<4; i++){
		//printk("Child i:%d\n", i);
		u64* curr = (u64*)(vaddr_base_child + arr[i]);
		//printk("curr : %x\n", curr);
		if(curr == NULL && i<3){
			//printk("Alloc\n");
			u64* temp = (u64*)os_pfn_alloc(OS_PT_REG);
			*curr = temp;
		}
		if(i==3){
			*curr = physical;
			get_pfn(osmap(physical & PAGE_MASK));
			break;
		}
		u64 entry = *curr;
		u64 newAddr = entry & PAGE_MASK;
		vaddr_base_child = newAddr;
		//if(i==3) physical = address;
	}
}

long do_cfork(){
    return -1; 
    u32 pid;
    struct exec_context *new_ctx = get_new_ctx();
    struct exec_context *ctx = get_current_ctx();
     /* Do not modify above lines
     * 
     * */   
     /*--------------------- Your code [start]---------------*/
    /*u32 child_pid = new_ctx->pid;	 
    *new_ctx = *ctx;
    new_ctx->pid = child_pid;
    new_ctx->ppid = ctx->pid;

    u64 new_pfn = os_pfn_alloc(OS_PT_REG);
    new_ctx->pgd = new_pfn;
    // create_new_page_table((u64*)osmap(ctx->pgd), (u64*)osmap(new_ctx->pgd), 0);
        
    struct mm_segment *childmm = new_ctx -> mms, *parentmm = ctx -> mms;
    childmm[MM_SEG_CODE] = parentmm[MM_SEG_CODE];
    childmm[MM_SEG_RODATA] = parentmm[MM_SEG_RODATA];
    childmm[MM_SEG_DATA] = parentmm[MM_SEG_DATA];
    childmm[MM_SEG_STACK] = parentmm[MM_SEG_STACK];

    struct vm_area* childva = new_ctx -> vm_area;

    struct vm_area* parentva = ctx -> vm_area;


    childva = (struct vm_area*) os_alloc(sizeof(struct vm_area));
    new_ctx->vm_area = childva;
    while (parentva != NULL) {
	*childva = *parentva;
	parentva = parentva -> vm_next; 
    	childva -> vm_next = (struct vm_area*) os_alloc(sizeof(struct vm_area));
	childva = childva -> vm_next;
    }

    //new_pfn = os_pfn_alloc(OS_PT_REG);
    //new_ctx -> pgd = new_pfn;

    for (u64 i = childmm[MM_SEG_CODE].start; i < childmm[MM_SEG_CODE].next_free; i+= 0x1000) {
    	    //printk("i:%x\n", i);
	    page_table_walk ((u64*)ctx -> pgd, (u64*) new_ctx -> pgd, i);
    
    }
    printk("1\n");
    
    for (u64 i = childmm[MM_SEG_RODATA].start; i < childmm[MM_SEG_RODATA].next_free; i+= 0x1000) {
    	page_table_walk ((u64*)ctx -> pgd, (u64*) new_ctx -> pgd, i);
    
    }
    printk("2\n");
    for (u64 i = childmm[MM_SEG_DATA].start; i < childmm[MM_SEG_DATA].next_free; i+= 0x1000) {
    	page_table_walk ((u64*)ctx -> pgd, (u64*) new_ctx -> pgd, i);
    
    }
    printk("3\n");

    for (u64 i = childmm[MM_SEG_STACK].start; i < childmm[MM_SEG_STACK].end; i+= 0x1000) {
    	page_table_walk ((u64*)ctx -> pgd, (u64*) new_ctx -> pgd, i);
    
    }
    printk("4\n");

    childva = new_ctx -> vm_area;
    parentva = ctx -> vm_area;

    // childva = (struct vm_area*) os_alloc(sizeof(struct vm_area));
    
    while (childva != NULL) {
	    printk(":cry:\n");
	    for (u64 i = childva -> vm_start; i < childva -> vm_end; i+= 0x1000) {
    		page_table_walk ((u64*)ctx -> pgd, (u64*) new_ctx -> pgd, i);
	    
	    } 
	    childva = childva -> vm_next; 
    }

    printk("Exited\n");
    */
     /*--------------------- Your code [end] ----------------*/
    
     /*
     * The remaining part must not be changed
     */
    copy_os_pts(ctx->pgd, new_ctx->pgd);
    do_file_fork(new_ctx);
    setup_child_context(new_ctx);
    reset_timer();

    return pid;
}


/* Cow fault handling, for the entire user address space
 * For address belonging to memory segments (i.e., stack, data) 
 * it is called when there is a CoW violation in these areas. 
 */

long handle_cow_fault(struct exec_context *current, u64 vaddr, int access_flags)
{
  	printk("Trying to COW\n");
	long retval = -1;

  return retval;
}
