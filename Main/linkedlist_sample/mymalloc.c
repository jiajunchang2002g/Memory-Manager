#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#include "llist.h"

char _heap[MEMSIZE] = {0};
TNode *_memlist = NULL;

long get_index(void *ptr) {
    if (ptr == NULL)
        return -1;
    else
        return (long)((char *)ptr - &_heap[0]);
}

void *mymalloc(size_t size) {
    if (size <= 0) return NULL;

    // starts with an empty node with full space
    if (!_memlist) {
        TData *initial = malloc(sizeof(TData));
        initial->start_addr = 0;
        initial->size = MEMSIZE;
        initial->status = 'F';

        TNode *node = make_node(initial->start_addr, initial);
        insert_node(&_memlist, node, ASCENDING);
    }

    reset_traverser(_memlist, FRONT);
    TNode *curr;
    // run down the llist
    while ((curr = succ(_memlist)) != NULL) {
        TData *block = curr->pdata;
        if (block->status == 'F' && block->size >= size) {
            if (block->size == size) {
                block->status = 'A';
                return &_heap[block->start_addr];
            } else {
                // Split the block
                TData *newblock = malloc(sizeof(TData));
                newblock->start_addr = block->start_addr + size;
                newblock->size = block->size - size;
                newblock->status = 'F';

                block->size = size;
                block->status = 'A';

                TNode *newnode = make_node(newblock->start_addr, newblock);
                insert_node(&_memlist, newnode, ASCENDING);

                return &_heap[block->start_addr];
            }
        }
    }

    return NULL;
}

void myfree(void *ptr) {
    if (!ptr) return;

    int offset = (char *)ptr - _heap;

    reset_traverser(_memlist, FRONT);
    TNode *curr;
    while ((curr = succ(_memlist)) != NULL) {
        TData *block = curr->pdata;
        if (block->start_addr == offset) {
            block->status = 'F';

            TNode *prev = curr->prev;
            if (prev) {
                TData *prevblock = prev->pdata;
                if (prevblock->status == 'F') {
                    merge_node(_memlist, curr, PRECEDING);
                    curr = prev;
                    block = curr->pdata;
                }
            }

            TNode *next = curr->next;
            if (next) {
                TData *nextblock = next->pdata;
                if (nextblock->status == 'F') {
                    merge_node(_memlist, curr, SUCCEEDING);
                }
            }

            return;
        }
    }
}

void print_memlist() {
    reset_traverser(_memlist, FRONT);
    TNode *curr;
    while ((curr = succ(_memlist)) != NULL) {
        TData *block = curr->pdata;
        printf("Start: %5d | Size: %5d | Status: %c\n",
               block->start_addr, block->size, block->status);
    }
}


