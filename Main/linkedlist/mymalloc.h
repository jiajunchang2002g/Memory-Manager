#include <stddef.h>

#define MEMSIZE (64 * 1024)       
 

long get_index(void *ptr);
void* mymalloc(size_t);
void myfree(void *);
void print_memlist();
