#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>
#define MAX_MEM 1<<30
#include <stdio.h>
#include <math.h>
struct Node{
	unsigned char header;
	struct Node* prev;
	struct Node* next;
};


struct Node *freeList[26] = {NULL};
static void *base = NULL;

void *my_buddy_malloc(int size){
	if(size > MAX_MEM){
		printf("Malloc size needs to be 1GB or less.\n");
		return NULL;
	}
	if(base == NULL){
		base = mmap(NULL, MAX_MEM, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, 0, 0);
		freeList[25] = base;
		freeList[25]->header = 30;
		freeList[25]->next = NULL;
		freeList[25]->prev = NULL;
	}

	struct Node *output = NULL;
	int index = (int)ceil(log2(size+1))-5;
	if(index<0)
		index=0;
	int origIndex = index;
	//if the attempted malloc already has a block of that size
	if(freeList[origIndex] != NULL){
		//if the level already has a head
		if (freeList[origIndex]->next == NULL){
		  output = freeList[origIndex];
		  freeList[origIndex] = NULL;
		}
		//if that level has more than just a head
		else{
		  output = freeList[origIndex];
		  freeList[origIndex] = freeList[origIndex]->next;
		  freeList[origIndex]->prev = NULL;
		}
		  output->next = NULL;
		  output->prev = NULL;
		  output->header = index+5;
		  output->header |= 128;
	}
	//a free block doesn't exist for that size
	else{
		//find the next best size, or index
		while(freeList[index] == NULL){
			++index;
		}
		//repeatedly split and create free blocks on each iteration until 1 above the original index
		while(index > origIndex){

			struct Node *newBlock = freeList[index];
			int mid = (1 << (index +5))/2;
			struct Node *buddy = (struct Node*)((char *)newBlock + mid);
			//if the new block has a next neighbor, then set the new head to be its neighbor and disconnect new block
			if(newBlock->next != NULL){
				freeList[index] = newBlock->next;
				newBlock->next = NULL;
			}
			//new block is the head, so disconnect the head
			else{
				freeList[index] = NULL;
			}
			//link the new block and its buddy
			newBlock->next = buddy;
			buddy->prev = newBlock;
			//decrement back up a freeList level
			--index;
			//place the newly linked blocks in the front of the freeList Level as the new head
			freeList[index] = newBlock;
			//update the size of the new block and its buddy
			newBlock->header = index + 5;
			buddy->header = index + 5;
		}
		//get a free block of the requested size, disconnect it from the list, and return it to the user
		output = freeList[index];
		freeList[index] = freeList[index]->next;
		freeList[index]->prev = NULL;
		output->next = NULL;
		output->prev = NULL;
		output->header = index+5;
		output->header |= 128;
	}
	//return the free block to the user - block will be NULL if the malloc isn't possible
	return (void *)((char *)output + 1);
}

void my_free(void *ptr)
{
	//block being freed
	struct Node *free = (struct Node*)((char *)ptr-1);
	
	//set first bit as 0, or free
	free->header&=127;
	//find location of buddy
	void *buddyVal = (((void *)((char *)ptr-1)-base) ^ (1 << free->header)) + base;
	struct Node *buddy = (struct Node *)buddyVal;
	
	//a buddy is available and is the same size as the block being freed
	while((buddy->header==free->header) && !(buddy->header>>7))
	{
		
		//separate buddy from its location in freeList
		if(buddy->prev!=NULL)
		{
			
			if(buddy->next==NULL)
			{
				buddy->prev->next=NULL;
				freeList[buddy->header-5]=NULL;
			}
			else
				buddy->prev->next=buddy->next;
		}
		else
			freeList[buddy->header-5]=buddy->next;
		if(buddy->next!=NULL)
			buddy->next->prev=buddy->prev;
		buddy->next=NULL;
		buddy->prev=NULL;
		//if buddy is before free based on its offset
		if(buddy<free)
			free=buddy;
		free->header++;
		//find the next buddy of the new free block
		buddyVal=(((void *)((char *)free)-base) ^ (1 << free->header)) + base;
		buddy=(struct Node *)buddyVal;
	}

	int freeIndex=free->header-5;
	//place the buddy in the freeList based on if the list has a head or not
	if(freeList[freeIndex]!=NULL)
	{
		freeList[freeIndex]->prev=free;
		free->next=freeList[freeIndex];
		freeList[freeIndex]=free;
		free->prev=NULL;
	}
	else
	{
		freeList[freeIndex]=free;
		free->prev=NULL;
		free->next=NULL;
	}
}

void dump_heap(){
	struct Node *currNode;
	int i=0;
	int size=0;
	for(i = 0; i < 26; i++){
		printf("%d->", i + 5);
		if(freeList[i] == NULL){
			printf("NULL\n");
		}
		else{
			currNode = freeList[i];
			while(currNode!=NULL){
				
				size = 1 << (currNode->header);
				printf("[%d, %d, %d]->", 0, (int)((char *)currNode - (char *)base), size);
				currNode = currNode->next;
			}
			printf("NULL\n");
		}
	}
	printf("\n\n");
}
