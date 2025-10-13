#include<tb.h>
#include<lib.h>
#include<file.h>
#include<context.h>
#include<memory.h>

/*
 * *  Trace buffer implementation.
 *
 */


///////////////////////////////////////////////////////////////////////////
////           TODO:     Trace buffer functionality                   /////
///////////////////////////////////////////////////////////////////////////


// Check whether passed buffer is valid memory location for read.
static int tb_validate(unsigned long buff, u32 count, int read_write) 
{
	struct exec_context *ctx = get_current_ctx();
	//read
	if(read_write == 0){
		int isValid = 0;
		if(ctx->mms[MM_SEG_DATA].start <= buff && ctx->mms[MM_SEG_DATA].next_free >= buff + count) isValid = 1;
		
		else if(ctx->mms[MM_SEG_RODATA].start <= buff && ctx->mms[MM_SEG_RODATA].next_free >= buff + count) isValid = 0;
		
		else if(ctx->mms[MM_SEG_CODE].start <= buff && ctx->mms[MM_SEG_CODE].next_free >= buff + count) isValid = 0;
		else if(ctx->mms[MM_SEG_STACK].start <= buff && ctx->mms[MM_SEG_STACK].end >= buff + count) isValid = 1;

		else{
			struct vm_area* vmax = ctx->vm_area;
			while(1){
				if(vmax->vm_start <= buff && vmax->vm_end >= buff+count) break;
				else if(vmax->vm_next != NULL){
					vmax = vmax->vm_next;
				}
				else{
					return -1;
				}
			}
			u32 ac_flags = vmax->access_flags;
			if(ac_flags%4 == 2 || ac_flags%4 == 3) isValid = 1;
		}
		
		if(isValid) return 1;
		return -1;
	}
	else if(read_write == 1){
		int isValid = 0;
		if(ctx->mms[MM_SEG_DATA].start <= buff && ctx->mms[MM_SEG_DATA].next_free >= buff + count) isValid = 1;
		
		else if(ctx->mms[MM_SEG_RODATA].start <= buff && ctx->mms[MM_SEG_RODATA].next_free >= buff + count) isValid = 1;
		
		else if(ctx->mms[MM_SEG_CODE].start <= buff && ctx->mms[MM_SEG_CODE].next_free >= buff + count) isValid = 1;
		
		else if(ctx->mms[MM_SEG_STACK].start <= buff && ctx->mms[MM_SEG_STACK].end >= buff + count) isValid = 1;

		else{
			//u32 ac_flags = ctx->vm_area->access_flags;
			//printk("Why I am run??????\n");
			struct vm_area* vmax = ctx->vm_area;
			while(1){
				if(vmax->vm_start <= buff && vmax->vm_end >= buff+count) break;
				else if(vmax->vm_next != NULL){
					vmax = vmax->vm_next;
				}
				else{
					return -1;
				}
			}
			u32 ac_flags = vmax->access_flags;
			//struct vm_area* vmax = ctx->vm_area;
			if(ac_flags%2 == 1) isValid = 1;
		}
		//else isValid = 1;
		
		if(isValid) return 1;
		return -1;
	}
	
	printk("Error\n");	
      return -1;
}

static long tb_close(struct file *filep)
{
        //printk("Error\n");	
        //return -1;
	filep = NULL;
	return 0;
}

static int tb_read(struct file *filep, char *buff, u32 count)
{

	if(tb_validate((long)buff, count, 0) == -1) return -EBADMEM;
	int available = filep->tb->write_ptr - filep->tb->read_ptr;
	//u32 temp = available;
	if(available < 0){
		available = filep->tb->write_ptr - filep->tb->base_ptr + filep->tb->size - (filep->tb->read_ptr - filep->tb->base_ptr);
	}
	u32 temp = (u32) available;

	u32 to_read = count;
	if(temp <= count) to_read = temp;
	
	struct tb_info* tbx = filep->tb;
	//printk("%d %d\n", tbx->read_ptr - tbx->base_ptr, tbx->write_ptr - tbx->base_ptr);
	//u32 temp = available;
	for(int i=0;i<to_read;i++){
		u32 curr_pos = (tbx->read_ptr - tbx->base_ptr)%(tbx->size);
		buff[i] = tbx->base_ptr[curr_pos];
		tbx->read_ptr++;
		if(tbx->read_ptr - tbx->base_ptr >= tbx->size) tbx->read_ptr -= tbx->size;
	}
	//printk("%d %d %d\n", to_read, tbx->read_ptr - tbx->base_ptr, tbx->write_ptr - tbx->base_ptr);
	return to_read;
}

static int tb_write(struct file *filep, char *buff, u32 count)
{
    	if(tb_validate((long)buff, count, 1) == -1) return -EBADMEM;
	if(filep->tb->write_ptr - filep->tb->base_ptr == 4096){
		filep->tb->write_ptr = filep->tb->base_ptr;
	}
	u32 available =(u32)(filep->tb->size - (filep->tb->write_ptr - filep->tb->read_ptr));
	u32 to_write = count;
	if(available <= count) to_write = available;
	
	struct tb_info* tbx = filep->tb;
	for(int i=0;i<to_write;i++){
		u32 curr_pos = (tbx->write_ptr - tbx->base_ptr)%(tbx->size);
		tbx->base_ptr[curr_pos] = buff[i];
		tbx->write_ptr++;
		if(tbx->write_ptr - tbx->base_ptr >= tbx->size) tbx->write_ptr -= tbx->size;
	}
	if (to_write > 0 && tbx->write_ptr == tbx->base_ptr) {
		tbx->write_ptr = tbx->base_ptr + tbx->size;
	}
	return to_write;
}

int sys_create_tb(struct exec_context *current, int mode)
{
	int ret_fd = -1;
	for(int i=0; i< MAX_OPEN_FILES; i++){
		if(current->files[i] == NULL){
			ret_fd = i;
			break;
		}
	}
	if(ret_fd == -1) return -EINVAL;
	struct file *filex = (struct file*)os_alloc(sizeof(struct file));
	current->files[ret_fd] = filex;
	struct tb_info *tbx = (struct tb_info*)os_alloc(sizeof(struct tb_info));
	
	int* buffer = (int*)os_page_alloc(USER_REG);
	tbx->base_ptr = buffer;
	tbx->read_ptr = buffer;
	tbx->write_ptr = buffer;
	tbx->size = TRACE_BUFFER_MAX_SIZE;

	filex->mode = mode;
	filex->tb = tbx;
	filex->ref_count = 1;
	
	struct fileops* opsx = (struct fileops*)os_alloc(sizeof(struct fileops));
	filex->fops = opsx;
	opsx->read = &tb_read;
	opsx->write = &tb_write;
	opsx->close = &tb_close;
    //printk("Error\n");	
        return ret_fd;
}
