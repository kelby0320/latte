#ifndef HEAP_H
#define HEAP_H

#include "config.h"

#include <stddef.h>

#define block_list_size(order)						\
    ((1 << (LATTE_HEAP_MAX_ORDER - order)) * LATTE_HEAP_BLOCK_MULTIPLIER)

typedef unsigned int HEAP_BLOCK_FLAGS;

#define HEAP_BLOCK_UNINITALIZED 0b00000000
#define HEAP_BLOCK_AVAILABLE 0b00000001
#define HEAP_BLOCK_ALLOCATED 0b00000010

struct block_item {
    void *addr;
    HEAP_BLOCK_FLAGS flags;
};

struct block_list {
    struct block_item *list;
    int len;			/* Number of blocks allocated or available */
    int num_alloc;		/* Number of blocks allocated */
    size_t size;
};

struct heap {
    struct block_item _order_0_list[block_list_size(0)];
    struct block_item _order_1_list[block_list_size(1)];
    struct block_item _order_2_list[block_list_size(2)];
    struct block_item _order_3_list[block_list_size(3)];
    struct block_item _order_4_list[block_list_size(4)];
    struct block_item _order_5_list[block_list_size(5)];
    struct block_item _order_6_list[block_list_size(6)];
    struct block_item _order_7_list[block_list_size(7)];
    struct block_item _order_8_list[block_list_size(8)];
    struct block_item _order_9_list[block_list_size(9)];
    struct block_item _order_10_list[block_list_size(10)];

    struct block_list block_lists[11];

    void *saddr;
};

int
heap_init(struct heap *heap, void *saddr);

void*
heap_malloc(struct heap *heap, size_t size);

void
heap_free(struct heap *heap, void *ptr);

#endif
