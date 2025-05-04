#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include "llist.h"

char _heap[MEMSIZE] = {0};
TNode *_memlist = NULL;

long get_index(void *ptr)
{
    if (ptr == NULL)
        return -1;
    else
        return (long)((char *)ptr - &_heap[0]);
}

void free_data(TNode *node)
{
    free(node->pdata);
}

TNode *search_list(TNode *llist, int size)
{
    reset_traverser(llist, FRONT);
    TNode *node;
    do
    {
        node = succ(llist);
        if (node)
        {
            if (node->pdata->is_empty && node->pdata->size >= size)
            {
                return node;
            }
        }
    } while (node != NULL);
    return NULL;
}

void *mymalloc(size_t size)
{
    if (size <= 0 || size > MEMSIZE)
        return NULL;

    // start with an empty node of full space
    if (!_memlist)
    {
        TData *data = malloc(sizeof(TData));
        data->size = MEMSIZE;
        data->start = 0;
        data->is_empty = 1;

        TNode *node = make_node(data->start, data);
        insert_node(&_memlist, node, ASCENDING);
    }

    TNode *node = search_list(_memlist, size);
    if (node)
    {
        TData *block = node->pdata;
        if (block->size == size)
        {
            block->is_empty = 0;
            return &_heap[block->start];
        }
        else if (block->size > size)
        {
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
        return NULL;
    }
}

void myfree(void *ptr)
{
    if (!ptr) return;
    reset_traverser(_memlist, FRONT);
    TNode *node = find_node(_memlist, get_index(ptr));

    if (!node) return;
    
    TData *block = node->pdata;
    block->is_empty = 1;

    TNode *prev = node->prev;
    if (prev) {
        TData *prevblock = prev->pdata;
        if (prevblock->is_empty) {
            merge_node(_memlist, node, PRECEDING);
            // update variables for later use
            node = prev;
            block = node->pdata;
        }
    }

    TNode *next = node->next;
        if (next) {
            TData *nextblock = next->pdata;
            if (nextblock->is_empty) {
                merge_node(_memlist, node, SUCCEEDING);
            }
        }
    return;
}

void myprint(TNode *node)
{
    TData *block = node->pdata;
    printf("Start: %5d | Size: %5zu | Status: %d\n",
           block->start, block->size, block->is_empty);
}

void print_memlist()
{
    process_list(_memlist, myprint);
}
