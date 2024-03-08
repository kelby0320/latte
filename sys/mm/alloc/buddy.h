#ifndef BUDDY_H
#define BUDDY_H

#include <stdbool.h>
#include <stddef.h>

#define BUDDY_BLOCK_MAX_ORDER 10
#define BUDDY_BLOCK_LIST_LEN  (1 << BUDDY_BLOCK_MAX_ORDER)
#define BUDDY_BLOCK_MIN_SIZE  4096
#define BUDDY_BLOCK_MAX_SIZE  (BUDDY_BLOCK_MIN_SIZE * (1 << BUDDY_BLOCK_MAX_ORDER))
#define order_to_size(order)  (BUDDY_BLOCK_MIN_SIZE * (1 << order))

/**
 * @brief
 *
 */
struct buddy_block {
    unsigned int order;
    void *addr;
    struct buddy_block *buddy_list[BUDDY_BLOCK_MAX_ORDER];
    bool is_allocated;
};

/**
 * @brief
 *
 */
struct buddy_allocator {
    struct buddy_block buddy_block_list[BUDDY_BLOCK_LIST_LEN];
    void *base_addr;
    size_t mem_available;
};

/**
 * @brief
 *
 * @param allocator
 * @param base_addr
 */
void
buddy_allocator_init(struct buddy_allocator *allocator, void *base_addr);

/**
 * @brief
 *
 * @param allocator
 * @param order
 * @return void*
 */
void *
buddy_allocator_malloc(struct buddy_allocator *allocator, unsigned int order);

/**
 * @brief
 *
 * @param allocator
 * @param addr
 */
void
buddy_allocator_free(struct buddy_allocator *allocator, void *addr);

#endif
