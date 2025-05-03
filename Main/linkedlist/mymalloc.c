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

void *mymalloc(size_t size) {
    if (size <= 0 || size > MEMSIZE) return NULL;

    if (_memlist == NULL) {
        // allocated space
        TData *data = malloc(sizeof(TData));
        data->size = size;
        data->start = 0;
        data->is_empty = 0;
        TNode *node = make_node(0, data);

        // free space
        TData *data2 = malloc(sizeof(TData));
        data2->size = MEMSIZE - size;
        data2->start = size;
        data2->is_empty = 1;
        TNode *node2 = make_node(size, data2);

        insert_node(&_memlist, node, ASCENDING);
        insert_node(&_memlist, node2, ASCENDING);

        return &_heap[0];
    }
    TNode *node = _memlist;
    while (node != NULL) {
        // check this node for space
        if (node->pdata->is_empty) {
            if (node->pdata->size > size) {
                // add free node
                TData *data2 = malloc(sizeof(TData));
                data2->size = node->pdata->size - size;
                data2->start = node->pdata->start + size;
                data2->is_empty = 1;
                TNode *node2 = make_node(data2->start, data2);
                insert_node(&_memlist, node2, ASCENDING);
                // allocate to node
                node->pdata->size = size;
                node->pdata->is_empty = 0;
            } else if (node->pdata->size == size) {
                // simple case
                node->pdata->is_empty = 0;
            }
            return &_heap[node->pdata->start];
        }
        // search next node
        node = succ(node);
    }
    // no space is found
    return NULL;
}

void myfree(void *ptr) {
    if (ptr == NULL) return;
    TNode *node = find_node(_memlist, get_index(ptr));
    if (node == NULL) return;
    node->pdata->is_empty = 1;
    // merge with succ
    if (succ(node) != NULL && succ(node)->pdata->is_empty == 1) {
        TNode *s = succ(node);
        node->pdata->size += s->pdata->size;

        delete_node(&_memlist, s);
        free(s->pdata);
    }
    // // merge with prev
    // if (pred(node) != NULL && pred(node)->pdata->is_empty == 1) {
    //     TNode *prev = pred(node);
    //     prev->pdata->size += node->pdata->size;

    //     delete_node(&_memlist, node);
    //     free(node->pdata);
    // }
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

void print_memlist() { process_list(_memlist, myprint); }

void print_node(TNode *node) {
    if(node != NULL) { 
        printf("STATUS: ");
        if (node->pdata->is_empty) printf("FREE ");
        else printf("ALLOCATED ");
        printf("Start index: %i Length: %lu\n", node->pdata->start,
        node->pdata->size);
        printf("Key: %d\n", node->key);
    }
    else
        printf("Unable to find key.\n");
}

void free_data(TNode *node) {
    free(node->pdata);
}

void trav_list(TNode *llist) {
    reset_traverser(llist, FRONT);
    TNode *node;

    do {
        node = succ(llist);
        if(node)
            print_node(node);
    } while(node != NULL);
}
