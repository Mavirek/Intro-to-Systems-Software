#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>
#define MAX_MEM 1<<30
#include <stdio.h>
#include <math.h>
#include "mymalloc.c"


void *my_buddy_malloc(int size);
void my_free(void *ptr);
void dump_heap(); 