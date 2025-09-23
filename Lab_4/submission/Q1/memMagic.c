#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>

struct node
{
	unsigned size;
	int *data;
	struct node *next;
};

int init_node(struct node* n, unsigned size)
{
	assert(n != NULL);
	//printf("size=%ud\n",size);
	n->size = size;
	n->next = NULL;
	n->data = mmap(NULL, sizeof(int)*size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
	assert(n->data > 0);
	//printf("Hi\n");
	for(unsigned i = 0; i < size; i++)
	{
		n->data[i] = 0;
	//	printf("%d\n",i);
	}

	return 0;
}

int clean_node(struct node* n)
{
	assert(n != NULL);
	munmap(n->data, sizeof(int)*n->size);

	return 0;
}

int inflate_node(struct node* n)
{
	unsigned new_size = 0;

	assert(n != NULL);
	new_size = 2 * n->size;
	int old_size = n->size;
	n->size = new_size;
	n->data = mremap(n->data, old_size * sizeof(int), sizeof(int)*new_size, MREMAP_MAYMOVE);	
	assert(n->data > 0);

	for(unsigned i = 0; i < new_size; i++)
	{
		n->data[i] = 0;
		//printf("%d\n",i);
	}

	return 0;
}


struct node *alloc_node()
{
	struct node *n = 0;

	//printf("size:%ld\n",sizeof(struct node));
	n = mmap(NULL, sizeof(struct node), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	assert(n > 0);

	return n;
}

int free_node(struct node *n)
{
	munmap(n, sizeof(struct node));
	return 0;
}

int main()
{
	struct node *head = 0;

	head = alloc_node();
	init_node(head, 1024);
	//printf("yes\n");
	for(int i = 0; i < 20; i++)
	{
		inflate_node(head);
		//printf("%d\n",i);
	}
	//printf("Clean\n");

	clean_node(head);
	//printf("free\n");
	free_node(head);

	printf("Program surgery successful!\n");

	return 0;
}

