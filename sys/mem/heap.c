#include "mem/heap.h"
#include "libk/memory.h"
#include "errno.h"

static void
allocate_initial_blocks(struct heap *heap)
{
    unsigned int offset = LATTE_HEAP_MIN_BLOCK_SIZE * (1 << LATTE_HEAP_MAX_ORDER);
    
    for (int i = 0; i < LATTE_HEAP_LARGE_BLOCKS; i++) {
        struct block_tree *block_tree = &heap->block_trees[i];
        struct block_list *order_10_list = &block_tree->block_lists[10];
        struct block_item *order_10_item = &order_10_list->list[0]; /* Only 1 item in order 10 */

        /* Mark item available and set its address */
        order_10_item->addr = heap->saddr + (offset * i);
        order_10_item->flags = HEAP_BLOCK_AVAILABLE;

        order_10_list->num_alloc_or_avail = 1;
    }
}

static int
size_to_order_n(size_t size)
{
    int order_n;
    
    for (order_n = 0; order_n <= LATTE_HEAP_MAX_ORDER; order_n++) {
        if ((size / (LATTE_HEAP_MIN_BLOCK_SIZE << order_n)) == 0) {
            return order_n;
        }
    }

    return -ENOMEM;
}

static struct block_item*
find_free_block(struct block_tree *block_tree, int order_n)
{
    struct block_list *block_list = &block_tree->block_lists[order_n];
    struct block_item *item = 0;

    /* Are there any items in this list? */
    if (block_list->num_alloc_or_avail == 0) {
	    return item;
    }

    /* Search the entire list for available blocks */
    for (size_t i = 0; i < block_list->size; i++) {
        if (block_list->list[i].flags & HEAP_BLOCK_AVAILABLE) {
            item = &block_list->list[i];
            break;
        }
    }

    return item; 
}

struct block_item*
find_uninitialized_block_pair(struct block_tree *block_tree, int order_n)
{
    struct block_list *block_list = &block_tree->block_lists[order_n];
    struct block_item *item = 0;

    /* Is this list already full? */
    if (block_list->num_alloc_or_avail == (int)block_list->size) {
	    return item;
    }

    for (size_t i = 0; i < block_list->size - 1; i++) {
        if (block_list->list[i].flags == 0) {
            /* One item is available */
            if (block_list->list[i+1].flags == 0) {
                /* We found two consecutive uninitialized items */
                item = &block_list->list[i];
                break;
            }
        }
    }

    return item;
}

static struct block_item*
add_block_pair(struct block_tree *block_tree, struct block_item *item, int order_n)
{
    struct block_list *block_list = &block_tree->block_lists[order_n];
    struct block_item *block_pair = find_uninitialized_block_pair(block_tree, order_n);
    if (!block_pair) {
        return NULL;
    }

    block_pair[0].addr = item->addr;
    block_pair[0].flags = HEAP_BLOCK_AVAILABLE;
    block_pair[1].addr = item->addr + LATTE_HEAP_MIN_BLOCK_SIZE * (1 << order_n);
    block_pair[1].flags = HEAP_BLOCK_AVAILABLE;

    block_list->num_alloc_or_avail += 2;

    return block_pair;
}

static void
remove_block(struct block_tree *block_tree, struct block_item *item, int order_n)
{
    struct block_list *block_list= &block_tree->block_lists[order_n];
    block_list->num_alloc_or_avail--;
    memset(item, 0, sizeof(struct block_item));
}

static struct block_item*
split_block(struct block_tree *block_tree, struct block_item *item, int order_n)
{
    struct block_item *res = add_block_pair(block_tree, item, order_n - 1);
    if (!res) {
        return NULL;
    }
    
    remove_block(block_tree, item, order_n);
    return res;
}

static struct block_item*
get_block_from_block_tree(struct block_tree *block_tree, int order_n)
{
    struct block_item *item = find_free_block(block_tree, order_n);
    if (!item) {
        /* 
            No free block or order_n was available.
            We are already at the highest order and cannot search further.
        */
        if (order_n == LATTE_HEAP_MAX_ORDER) {
            return NULL;
        }

        /*
            Try to find a free block in order_n + 1.
        */
        item = get_block_from_block_tree(block_tree, order_n + 1);
        if (!item) {
            return NULL;    /* Out of memory */
        }

        /*
            We have a block of order_n + 1, split the
            block into two blocks of order_n.
        */
        item = split_block(block_tree, item, order_n + 1);
    }

    return item;
}

static struct block_item*
get_block(struct heap *heap, int order_n)
{
    for (int i = 0; i < LATTE_HEAP_LARGE_BLOCKS; i++) {
        struct block_tree *block_tree = &heap->block_trees[i];
        struct block_item *item = get_block_from_block_tree(block_tree, order_n);
        if (item) {
            return item;
        }
    }

    return NULL;
}

/* static struct block_item* */
/* find_allocated_ptr_in_list(struct block_list *block_list, void *ptr) */
/* { */
/*     struct block_item *item = 0; */

/*     for (size_t i = 0; i < block_list->size; i++) { */
/* 	if (block_list->list[i].addr == ptr) { */
/* 	    item = &block_list->list[i]; */
/* 	    break; */
/* 	} */
/*     } */

/*     return item; */
/* } */

/* static int */
/* find_allocated_ptr(struct heap *heap, void *ptr, struct block_item **item) */
/* { */
/*     int order_n; */
/*     struct block_item *item = 0; */
    
/*     for (order_n = 0; i < LATTE_HEAP_MAX_ORDER + 1; order_n++) { */
/* 	struct block_list *block_list = heap->block_lists[order_n]; */

/* 	/\* Skip looking at this list if there are no items allocated *\/ */
/* 	if (block_list->num_alloc == 0) { */
/* 	    continue; */
/* 	} */

/* 	/\* Try to find the item in this list  *\/ */
/* 	item = find_allocated_ptr_in_list(block_list, ptr); */
/* 	if (item) { */
/* 	    break; */
/* 	} */
/*     } */

/*     *item = item; */
/*     return order_n; */
/* } */

/* static struct block_item* */
/* find_buddy_block(struct heap *heap, struct block_item *item, int order_n) */
/* { */
/*     struct block_item *buddy = 0; */
/*     unsigned int order_n_block_size = LATTE_HEAP_MIN_BLOCK_SIZE * (1 << order_n); */
/*     unsigned int order_n_plus_one_block_size = LATTE_HEAP_MIN_BLOCK_SIZE * (1 << order_n + 1); */
    
/*     if (order_n == LATTE_HEAP_MAX_ORDER) { */
/* 	return NULL; */
/*     } */
    
/*     unsigned int is_aligned = item->addr % order_n_plus_one_block_size; */
/*     if (aligned) { */
/* 	buddy = item->addr + order_n_block_size; */
/*     } else { */
/* 	buddy = item->addr - order_n_block_size; */
/*     } */

/*     return buddy; */
/* } */

/* static struct block_item* */
/* merge_blocks(struct heap *heap, struct block_item *item, struct block_item *buddy, int order_n) */
/* { */
/*     /\* */
/*       Remove item and buddy */
/*       Add new block of order_n+1 at min_addr(item, buddy) */
/*       return new block */
/*     *\/ */
/*     remove_block(heap, item, order_n); */
/*     remove_block(heap, buddy, order_n); */
/* } */

static void
block_tree_init(struct block_tree *block_tree)
{
    /* Initialize all block lists to zero  */
    memset(block_tree->_order_0_list, 0, sizeof(block_tree->_order_0_list));
    memset(block_tree->_order_1_list, 0, sizeof(block_tree->_order_1_list));
    memset(block_tree->_order_2_list, 0, sizeof(block_tree->_order_2_list));
    memset(block_tree->_order_3_list, 0, sizeof(block_tree->_order_3_list));
    memset(block_tree->_order_4_list, 0, sizeof(block_tree->_order_4_list));
    memset(block_tree->_order_5_list, 0, sizeof(block_tree->_order_5_list));
    memset(block_tree->_order_6_list, 0, sizeof(block_tree->_order_6_list));
    memset(block_tree->_order_7_list, 0, sizeof(block_tree->_order_7_list));
    memset(block_tree->_order_8_list, 0, sizeof(block_tree->_order_8_list));
    memset(block_tree->_order_9_list, 0, sizeof(block_tree->_order_9_list));
    memset(block_tree->_order_10_list, 0, sizeof(block_tree->_order_10_list));

    /* Setup block list pointers */
    block_tree->block_lists[0].list = block_tree->_order_0_list;
    block_tree->block_lists[1].list = block_tree->_order_1_list;
    block_tree->block_lists[2].list = block_tree->_order_2_list;
    block_tree->block_lists[3].list = block_tree->_order_3_list;
    block_tree->block_lists[4].list = block_tree->_order_4_list;
    block_tree->block_lists[5].list = block_tree->_order_5_list;
    block_tree->block_lists[6].list = block_tree->_order_6_list;
    block_tree->block_lists[7].list = block_tree->_order_7_list;
    block_tree->block_lists[8].list = block_tree->_order_8_list;
    block_tree->block_lists[9].list = block_tree->_order_9_list;
    block_tree->block_lists[10].list = block_tree->_order_10_list;

    /* Setup initial values for each block_list */
    for (int i = 0; i <= LATTE_HEAP_MAX_ORDER; i++) {
	block_tree->block_lists[i].num_alloc_or_avail = 0;
	block_tree->block_lists[i].num_alloc = 0;
	block_tree->block_lists[i].size = block_list_size(i);
    }
}

static void*
heap_malloc_large_blocks(struct heap *heap, size_t size)
{
    /* TODO */
    return NULL;
}

int
heap_init(struct heap *heap, void *saddr)
{
    /* Initialize all block lists */
    for (int i = 0; i < LATTE_HEAP_LARGE_BLOCKS; i++) {
	    block_tree_init(&heap->block_trees[i]);
    }
    
    /* Set heap start address  */
    heap->saddr = saddr;

    allocate_initial_blocks(heap);

    return 0;
}

void*
heap_malloc(struct heap *heap, size_t size)
{
    if (size > LATTE_HEAP_MAX_BLOCK_SIZE) {
	    return heap_malloc_large_blocks(heap, size);
    }
    
    int order_n = size_to_order_n(size);
    if (order_n == -ENOMEM) {
	    return NULL;
    }
    
    struct block_item *item = get_block(heap, order_n);
    if (!item) {
	    return NULL;
    }

    item->flags = HEAP_BLOCK_ALLOCATED;
    return item->addr;
}

/* void */
/* heap_free(struct heap *heap, void *ptr) */
/* { */
/*     struct block_item *item = 0; */
/*     int order_n = find_allocated_ptr(heap, ptr, &item); */
/*     if (!item) { */
/* 	return;			/\* Couldn't find the ptr *\/ */
/*     } */

/*     item->flags = HEAP_BLOCK_AVAILABLE; */

/*     struct block_item *buddy = find_buddy_block(heap, item, order_n); */
/*     while (item->flags & HEAP_BLOCK_AVAILABLE */
/* 	   && buddy */
/* 	   && buddy->flags & HEAP_BLOCK_AVAILABLE) { */
/* 	item = merge_blocks(heap, item, buddy, order_n); */
/* 	buddy = find_buddy_block(heap, item, order_n); */
/* 	order_n++; */
/*     } */
/* } */
