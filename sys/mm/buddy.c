#include "mm/buddy.h"

#include "errno.h"
#include "libk/memory.h"

/**
 * @brief Initialize a block list
 *
 * @param block_list    The block list
 * @param base_addr     The base address of the block list
 */
static void
block_list_init(struct buddy_block *block_list, void *base_addr)
{
    block_list[0].order = BUDDY_BLOCK_MAX_ORDER;
    block_list[0].addr = base_addr;
    memset(block_list[0].buddy_list, 0, sizeof(block_list[0].buddy_list));
    block_list[0].is_allocated = false;
}

/**
 * @brief   Find a free block
 *
 * @param block_list            The block list
 * @return struct buddy_block*  The free block
 */
static struct buddy_block *
block_list_new_block(struct buddy_block *block_list)
{
    for (int i = 0; i < BUDDY_BLOCK_LIST_LEN; i++) {
        if (block_list[i].addr == 0) {
            return &block_list[i];
        }
    }

    return NULL;
}

/**
 * @brief   Remove a block from a block list
 *
 * @param block_list    The block list
 * @param block         The block to remove
 */
static void
block_list_remove_block(struct buddy_block *block_list, struct buddy_block *block)
{
    for (int i = 0; i < BUDDY_BLOCK_LIST_LEN; i++) {
        if (&block_list[i] == block) {
            memset(&block_list[i], 0, sizeof(struct buddy_block));
            return;
        }
    }
}

/**
 * @brief   Find a block by its address
 *
 * @param block_list            The block list
 * @param addr                  The address to find
 * @return struct buddy_block*  The block or NULL
 */
static struct buddy_block *
block_list_find_block(struct buddy_block *block_list, void *addr)
{
    for (int i = 0; i < BUDDY_BLOCK_LIST_LEN; i++) {
        if (block_list[i].addr == addr) {
            return &block_list[i];
        }
    }

    return NULL;
}

/**
 * @brief   Find a free block of a given order
 *
 * @param block_list            The block list
 * @param order                 The order of the block
 * @return struct buddy_block*  The free block or NULL
 */
static struct buddy_block *
block_list_find_free_block(struct buddy_block *block_list, unsigned int order)
{
    for (int i = 0; i < BUDDY_BLOCK_LIST_LEN; i++) {
        if (block_list[i].order == order && !block_list[i].is_allocated) {
            return &block_list[i];
        }
    }

    return NULL;
}

/**
 * @brief   Split a block into a pair of buddy blocks
 *
 * @param allocator The buddy allocator
 * @param block     The block to split
 * @return int      Status Code
 */
static int
split_block(struct buddy_allocator *allocator, struct buddy_block *block)
{
    unsigned int new_order = block->order - 1;

    struct buddy_block *buddy_block = block_list_new_block(allocator->buddy_block_list);
    if (!buddy_block) {
        return -ENOMEM;
    }

    buddy_block->order = new_order;
    buddy_block->addr = block->addr + (BUDDY_BLOCK_MIN_SIZE * (1 << new_order));
    buddy_block->buddy_list[new_order] = block;

    block->order -= new_order;
    block->buddy_list[new_order] = buddy_block;

    return 0;
}

/**
 * @brief   Merge a block with its buddy
 *
 * @param allocator The buddy allocator
 * @param block     The block to merge
 * @return struct buddy_block*  The merged block
 */
static struct buddy_block *
merge_block(struct buddy_allocator *allocator, struct buddy_block *block)
{
    struct buddy_block *buddy = block->buddy_list[block->order];
    unsigned int next_order = block->order + 1;

    struct buddy_block *merged_block = block;
    struct buddy_block *remove_block = buddy;

    // Either block or buddy will be the new merged block
    // Pick the one with a buddy_list[next_order] entry
    // The other will be NULL
    if (buddy->buddy_list[next_order] != NULL) {
        merged_block = buddy;
        remove_block = block;
    }

    merged_block->order = next_order;

    block_list_remove_block(allocator->buddy_block_list, remove_block);

    return merged_block;
}

/**
 * @brief   Allocate a block of a given order
 *
 * @param allocator The buddy allocator
 * @param order     The order of the block
 * @return struct buddy_block*  The allocated block or NULL
 */
static struct buddy_block *
allocate_block_of_order(struct buddy_allocator *allocator, unsigned int order)
{
    struct buddy_block *block = block_list_find_free_block(allocator->buddy_block_list, order);
    if (!block) {
        // No free block was available
        // We are already at the highest order and cannot search further
        if (order == BUDDY_BLOCK_MAX_ORDER) {
            return NULL;
        }

        // Try to find a free block in order_n + 1
        block = allocate_block_of_order(allocator, order + 1);
        if (!block) {
            return NULL; // Out of memory
        }

        int res = split_block(allocator, block);
        if (res < 0) {
            return NULL; // Out of memory
        }
    }

    return block;
}

/**
 * @brief   Deallocate a block
 *
 * @param allocator     The buddy allocator
 * @param block         The block to deallocate
 */
static void
deallocate_block(struct buddy_allocator *allocator, struct buddy_block *block)
{
    if (block->order == BUDDY_BLOCK_MAX_ORDER) {
        return;
    }

    struct buddy_block *buddy = block->buddy_list[block->order];
    if (buddy->is_allocated) {
        return;
    }

    struct buddy_block *merged_block = merge_block(allocator, block);

    deallocate_block(allocator, merged_block);
}

void
buddy_allocator_init(struct buddy_allocator *allocator, void *base_addr)
{
    memset(allocator->buddy_block_list, 0, sizeof(allocator->buddy_block_list));
    block_list_init(allocator->buddy_block_list, base_addr);

    allocator->base_addr = base_addr;
    allocator->mem_available = BUDDY_BLOCK_MAX_SIZE;
}

void *
buddy_allocator_malloc(struct buddy_allocator *allocator, unsigned int order)
{
    if (order > BUDDY_BLOCK_MAX_ORDER) {
        return NULL;
    }

    struct buddy_block *block = allocate_block_of_order(allocator, order);
    if (!block) {
        return NULL;
    }

    block->is_allocated = true;

    allocator->mem_available -= order_to_size(order);

    return block->addr;
}

void
buddy_allocator_free(struct buddy_allocator *allocator, void *addr)
{
    struct buddy_block *block = block_list_find_block(allocator->buddy_block_list, addr);
    if (!block) {
        return;
    }

    block->is_allocated = false;
    allocator->mem_available += order_to_size(block->order);

    deallocate_block(allocator, block);
}