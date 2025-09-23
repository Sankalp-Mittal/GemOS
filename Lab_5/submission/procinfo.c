#include<procinfo.h>
#include<lib.h>
#include<file.h>
#include<context.h>

static char* segment_names[MAX_MM_SEGS+1] = {"code", "rodata", "data", "stack", "invalid"}; 
static char* file_types[MAX_FILE_TYPE+1] = {"stdin", "stdout", "stderr", "reg", "pipe", "invalid"}; 

long get_process_info(struct exec_context *ctx, long cmd, char *ubuf, long len)
{
    long retval = -EINVAL;	
    /*
     * TODO your code goes in here
     * */
    if(cmd == GET_PINFO_GEN){
	    u32 pidx = ctx->pid;
	    u32 ppidx = ctx->ppid;
	    unsigned long pcb_addrx = (unsigned long)ctx;
	    char pnamex[CNAME_MAX];
	    strcpy(pnamex,ctx->name);
	    if(ubuf == NULL) return -EINVAL;
	    long curr_len = 0;
	    curr_len += sizeof(u32)*2;
	    curr_len += sizeof(unsigned long);
	    curr_len += CNAME_MAX*sizeof(char);
	    if(curr_len > len) return -EINVAL;
	    //if(len<1024) return -EINVAL;
	    struct general_info* temp = (struct general_info *) ubuf;
	    temp->pid = pidx;
	    temp->ppid = ppidx;
	    temp->pcb_addr = pcb_addrx;
	    //temp.pname = pnamex;
	    strcpy(temp->pname,pnamex);
	    //printk("%d, %d, %d\n",((struct general_info *)ubuf)->pid,-10,-10);
	    // ubuf = (char*)&temp;
	    return 1;
    }
    else if(cmd == GET_PINFO_FILE){
	    if(ubuf==NULL) return -EINVAL;
	    struct file_info* ret_files;
	    struct file** filesx = ctx->files;
	    int i=0;
	    int valid = 0;
	    int curr_len = 0;
	    const int SIZEE = 3*sizeof(u32) + 8 * sizeof(char);
	    while(i<MAX_OPEN_FILES){
		    if(filesx[i]==NULL){
			    i++;
			    continue;
		    }
		    valid++;
		    u32 modex = filesx[i]->mode;
		    u32 ref_countx = filesx[i]->ref_count;
		    //printk("refs:%d\n",ref_countx);
		    u32 fileposx = filesx[i]->offp;
		    char file_typex[8];
		    strcpy(file_typex,file_types[filesx[i]->type]);
		    curr_len += SIZEE;
		    if(curr_len > len) return -EINVAL;
			    char tempbuf[1024];
			    struct file_info* temp = (struct file_info*)(ubuf+curr_len-SIZEE);
			    temp -> mode = modex;
			    temp -> ref_count = ref_countx;
			    temp -> filepos = fileposx;
			    strcpy(temp -> file_type, file_typex);
		    //strcpy (ubuf + curr_len - SIZEE, tempbuf);
		    //printk("cp refs:%d\n",((struct file_info*)(ubuf+curr_len-SIZEE))->ref_count);
		    i++;
	    }

	    return valid;

    }

    else if (cmd == GET_PINFO_MSEG) {
	    if (ubuf == NULL) return -EINVAL;

	    struct mem_segment_info* ret_files;
	    struct mm_segment* filesx = ctx->mms;
	    int i=0;
	    int valid = 0;
	    int curr_len = 0;
	    const int SIZEE = 3*sizeof(u64) + 16 * sizeof(char);
	    while(i<MAX_MM_SEGS){
		    if(&filesx[i]==NULL){
			    i++;
			    continue;
		    }
		    valid++;
		    u64 startx = filesx[i].start;
		    u64 endx = filesx[i].end;
		    //printk("refs:%d\n",ref_countx);
		    u64 nextx = filesx[i].next_free;
		    char segnamex[8];
		    //strcpy(segnamex, segment_names[i]);
		    //printk("DEBUG: %s\n", segnamex);
		    // printk("debug: %s\n", permx);
		    char permx[4] = "___";
		    u32 perm_in = filesx[i].access_flags;
		    if(perm_in %2) permx[0] = 'R';
		    perm_in/=2;
		    if(perm_in%2) permx[1] = 'W';
		    perm_in/=2;
		    if(perm_in%2) permx[2] = 'X';
		    //strcpy(file_typex,file_types[filesx[i]->type]);
		    curr_len += SIZEE;
		    if(curr_len > len) return -EINVAL;
		    char tempbuf[1024];
		    // printk("debug: %s\n", permx);
		    struct mem_segment_info* temp = (struct mem_segment_info*)(ubuf+curr_len-SIZEE);
		    temp -> start = startx;
		    temp -> end = endx;
		    temp -> next_free = nextx;
		    strcpy(temp -> perm, permx);
		    strcpy(temp -> segname, segment_names[i]);
			    
			    // strcpy(temp -> file_type, file_typex);
		    //strcpy (ubuf + curr_len - SIZEE, tempbuf);
		    //printk("cp refs:%d\n",((struct file_info*)(ubuf+curr_len-SIZEE))->ref_count);
		    i++;
	    }

	    return valid;

    }


    else if (cmd == GET_PINFO_VMA) {
	    if (ubuf == NULL) return -EINVAL;

	    struct vm_area_info* ret_vma;
	    struct vm_area* vmasx = ctx->vm_area;
	    vmasx = vmasx->vm_next;
	    //int i=0;
	    int valid = 0;
	    int curr_len = 0;
	    const int SIZEE = 2*sizeof(u64) + 8 * sizeof(char);
	    while(vmasx != NULL){
		    valid++;
		    u64 startx = vmasx->vm_start;
		    u64 endx = vmasx->vm_end;
		    //printk("refs:%d\n",ref_countx);
		    // nextx = filesx[i].next_free;
		    //char segnamex[8];
		    //strcpy(segnamex, segment_names[i]);
		    //printk("DEBUG: %s\n", segnamex);
		    // printk("debug: %s\n", permx);
		    char permx[4] = "___";
		    u32 perm_in = vmasx->access_flags;
		    if(perm_in %2) permx[0] = 'R';
		    perm_in/=2;
		    if(perm_in%2) permx[1] = 'W';
		    perm_in/=2;
		    if(perm_in%2) permx[2] = 'X';
		    //strcpy(file_typex,file_types[filesx[i]->type]);
		    curr_len += SIZEE;
		    if(curr_len > len) return -EINVAL;
		    char tempbuf[1024];
		    // printk("debug: %s\n", permx);
		    struct vm_area_info* temp = (struct vm_area_info*)(ubuf+curr_len-SIZEE);
		    temp -> start = startx;
		    temp -> end = endx;
		    //temp -> next_free = nextx;
		    strcpy(temp -> perm, permx);
		    //strcpy(temp -> segname, segment_names[i]);
			    
			    // strcpy(temp -> file_type, file_typex);
		    //strcpy (ubuf + curr_len - SIZEE, tempbuf);
		    //printk("cp refs:%d\n",((struct file_info*)(ubuf+curr_len-SIZEE))->ref_count);
		    vmasx = vmasx->vm_next;
	    }

	    return valid;

    }
    return 1;    
}
