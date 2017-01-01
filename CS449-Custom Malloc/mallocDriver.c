#include "mymalloc.c"
#include <stdio.h>
#include <math.h>

int main()
{
	//dump_heap();
	
	void *pointer = my_buddy_malloc(500000000); 
	void *pointer2 = my_buddy_malloc(5000);
	void *pointer3 = my_buddy_malloc(500);
	dump_heap();
	my_free(pointer);
	dump_heap();
	my_free(pointer2);
	dump_heap();
	my_free(pointer3);
	dump_heap();
	
	/*
	char *block1 = my_buddy_malloc(1);
	//dump_heap();
	char *block2 = my_buddy_malloc(1);
	//dump_heap();
	char *block3 = my_buddy_malloc(1);
	dump_heap();
	my_free(block1);
	dump_heap();
	my_free(block2);
	dump_heap();
	my_free(block3);
	dump_heap();
	*/
	return 0;
}