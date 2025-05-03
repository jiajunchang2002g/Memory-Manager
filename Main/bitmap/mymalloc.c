#include "mymalloc.h"
#include "llist.h"
#include "bitmap.h"
#include <stdio.h>
#include <stdlib.h>



char _heap[MEMSIZE] = {0};
static TNode *alloc_list = NULL;

long get_index(void *ptr) {
    if (ptr == NULL)
        return -1;
    else
        return (long)((char *)ptr - &_heap[0]);
}

void print_memlist() {
    // Implement this to call print_map from bitmap.c
    print_map(_heap, 8);
}

void *mymalloc(size_t size) {
    TData *data = (TData *)malloc(sizeof(TData));
    data->len = size;
    int start = search_map(_heap, 8, size);
    // quietly return if no space is available
    if (start == -1) {
        free(data);
        return NULL;
    }
    // mark memory as used
    allocate_map(_heap, start, size);
    // move pointer to empty part of heap
    void *ptr = &_heap[start];
    int index = get_index(ptr);
    TNode *node = make_node(index, data);
    // add node to llist
    insert_node(&alloc_list, node, ASCENDING);
    return ptr;
}

void myfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    int index = get_index(ptr);
    TNode *node = find_node(alloc_list, index);
    if (node == NULL) {
        return;
    }

    free_map(_heap, index, node->pdata->len);
    free(node->pdata);
    delete_node(&alloc_list, node);
}