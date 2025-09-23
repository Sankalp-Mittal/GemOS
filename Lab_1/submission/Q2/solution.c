#include <stdio.h>
#include <stdlib.h>
#include "buffer_api.h"

int main(int argc, char* argv[]) {
	if(argc != 2){
		perror("usage: ./solution <testcase number>");
		return -1;
	}
	buffer_init(atoi(argv[1]));
	const struct ring_buffer *rb = buffer_get_base();

	/* ------ YOUR CODE ENDS HERE ------*/
	u64 data_tail_pos = (rb->data_tail)%(rb->data_size);
	//u64 data_head_pos = (rb->data_head)%(rb->data_size);
	void* data_ptr = rb->data_base + data_tail_pos;
	// void* tail_ptr_ps 
	//void* data_ptr_end = rb->data_base + data_head_pos;
	u64 num_lost_records = 0;
	u64 unknown_size = 0;
	u64 curr_len = rb->data_tail;
	while (curr_len < rb->data_head){
		struct sample_event* new_ptr = (struct sample_event*) data_ptr;
		u32 curr_type = (new_ptr->header).type;
		if (curr_type == PERF_RECORD_SAMPLE){
			printf("%p\n",(void *) new_ptr->addr);
		}
		else if (curr_type == PERF_RECORD_LOST){
			struct lost_event* lost_event_ptr = (struct lost_event*) data_ptr;
			num_lost_records+= lost_event_ptr->lost;
		}
		else{
			unknown_size += (new_ptr->header).size;
		}
		data_tail_pos = (data_tail_pos + (new_ptr->header).size)%(rb->data_size);
		data_ptr = rb->data_base + data_tail_pos;
		curr_len += (new_ptr->header).size;
	}
	/* print formats */
	// printf("0x%...."); // to print sample addresses
	printf("number of lost records: %ld\n", num_lost_records);
	printf("unknown size: %ld\n",unknown_size);

	/* ------ YOUR CODE ENDS HERE ------*/
	buffer_exit();
	return 0;
}

