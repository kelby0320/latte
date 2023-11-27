#ifndef HEAP_H
#define HEAP_H

#include "config.h"

#include <stddef.h>

#define block_list_size(order)						\
    (1 << (LATTE_HEAP_MAX_ORDER - order))

typedef unsigned char HEAP_BLOCK_FLAGS;

#define HEAP_BLOCK_AVAILABLE 0b00000001
#define HEAP_BLOCK_ALLOCATED 0b00000010
#define HEAP_BLOCK_MULTI_BLOCK 0b00000100

/**
 * @brief Memory block to be allocated
 * 
 */
struct block_item {
    // Memory address
    void *addr;

    // Flags
    HEAP_BLOCK_FLAGS flags;
};

/**
 * @brief List of blocks
 * 
 */
struct block_list {
    // Pointer to actually list
    struct block_item *list;

    // Number of blocks allocated or available
    int num_alloc_or_avail;

    // Number of blocks allocated 
    int num_alloc;

    // List size
    size_t size;
};

/**
 * @brief Block tree structure
 * 
 */
struct block_tree {
    // Order 0 through 10 block lists
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

    // Pointer to block lists
    struct block_list block_lists[11];

    // Start address
    void *saddr;
};

/**
 * @brief Heap
 * 
 * Managed using a buddy allocator scheme
 * 
 */
struct heap {
    // List of block trees
    struct block_tree block_trees[LATTE_HEAP_LARGE_BLOCKS];

    // Start address
    void *saddr;
};

/**
 * @brief Initialize a new heap
 * 
 * @param heap      Pointer to heap structure
 * @param saddr     Starting address of the heap
 * @return int      Status code
 */
int
heap_init(struct heap *heap, void *saddr);

/**
 * @brief Allocate memory from the heap
 * 
 * @param heap      Pointer to the heap
 * @param size      Size to allocation
 * @return void*    Pointer to memory or 0
 */
void*
heap_malloc(struct heap *heap, size_t size);

/**
 * @brief Free allocated memory
 *
 * @param heap      Pointer to the heap
 * @param ptr       Pointer to memory
 */
void
heap_free(struct heap *heap, void *ptr);

#endif
