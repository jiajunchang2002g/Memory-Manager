#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include "llist.h"

char _heap[MEMSIZE] = {0};
TNode *_memlist = NULL;

long get_index(void *ptr) {
    if (ptr == NULL)
        return -1;
    else
        return (long)((char *)ptr - &_heap[0]);
}

void free_data(TNode *node) {
    free(node->pdata);
}

TNode* search_list(TNode *llist, int size) {
    reset_traverser(llist, FRONT);
    TNode *node;
    do {
        node = succ(llist);
        if(node) {
            if(node->pdata->is_empty && node->pdata->size >= size) {
                return node;
            }
        }
    } while(node != NULL);
}

void *mymalloc(size_t size) {
    if (size <= 0 || size > MEMSIZE) return NULL;

    if (_memlist == NULL) {
        // allocated space
        TData *data = malloc(sizeof(TData));
        data->size = MEMSIZE;
        data->start = 0;
        data->is_empty = 1;

        TNode *node = make_node(data->start, data);
        insert_node(&_memlist, node, ASCENDING);
    }

    TNode* node = search_list(_memlist, size);
    // no free node found
    if (!node) return NULL;
    // free node found
    TData *block = node->pdata;
    if (block->size == size) {
        block->is_empty = 0;
        return &_heap[block->start];
    } else {
        // split block
        TData *newblock = malloc(sizeof(TData));
                newblock->start = block->start + size;
                newblock->size = block->size - size;
                newblock->is_empty = 1;

                block->size = size;
                block->is_empty = 0;

                TNode *newnode = make_node(newblock->start, newblock);
                insert_node(&_memlist, newnode, ASCENDING);

                return &_heap[block->start];
    }
}

void myfree(void *ptr) {
    if (!ptr) return;
    
    TNode *node = find_node(_memlist, get_index(ptr));
    if (node == NULL) return;
    node->pdata->is_empty = 1;
    // merge with succ
    if (succ(node) != NULL && succ(node)->pdata->is_empty == 1) {
        TNode *s = succ(node);
        node->pdata->size += s->pdata->size;

        delete_node(&_memlist, s);
        free_data(s);
    }
    return;
}

void myprint(TNode *node) {
    printf("STATUS: ");
    if (node->pdata->is_empty) {
        printf("FREE ");
    } else {
        printf("ALLOCATED ");
    }
    printf("Start index: %i Length: %lu\n", node->pdata->start,
           node->pdata->size);
}

void print_memlist() { 
    process_list(_memlist, myprint); 
}

