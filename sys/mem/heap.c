#include "mem/heap.h"
#include "libk/memory.h"
#include "errno.h"

static void
allocate_initial_blocks(struct heap *heap)
{
    /* Allocate all 4Mb blocks in the order 10 list */
    struct block_list *order_10_list = &heap->block_lists[10];
    unsigned int offset = LATTE_HEAP_MIN_BLOCK_SIZE * (1 << LATTE_HEAP_MAX_ORDER);

    for (size_t i = 0; i < order_10_list->size; i++) {
	order_10_list->list[i].addr = (void*)(heap->saddr + (i * offset));
	order_10_list->list[i].flags = HEAP_BLOCK_AVAILABLE;
    }

    order_10_list->len = order_10_list->size;
}

static int
size_to_order_n(size_t size)
{
    int order_n;
    
    for (order_n = 0; order_n <= LATTE_HEAP_MAX_ORDER; order_n++) {
	if ((order_n / size) == 0) {
	    return order_n;
	}
    }

    return -ENOMEM;
}

static struct block_item*
find_free_block(struct heap *heap, int order_n)
{
    struct block_list *block_list = &heap->block_lists[order_n];
    struct block_item *item = 0;

    /* Are there any items in this list? */
    if (block_list->len == 0) {
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
find_uninitialized_block_pair(struct heap *heap, int order_n)
{
    struct block_list *block_list = &heap->block_lists[order_n];
    struct block_item *item = 0;

    /* Is this list already full? */
    if (block_list->len == (int)block_list->size) {
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
add_block_pair(struct heap *heap, struct block_item *item, int order_n)
{
    struct block_list *block_list = &heap->block_lists[order_n];
    struct block_item *block_pair = find_uninitialized_block_pair(heap, order_n);
    if (!block_pair) {
	return NULL;
    }

    block_pair[0].addr = item->addr;
    block_pair[0].flags = HEAP_BLOCK_AVAILABLE;
    block_pair[1].addr = item->addr + LATTE_HEAP_MIN_BLOCK_SIZE * (1 << order_n);
    block_pair[1].flags = HEAP_BLOCK_AVAILABLE;

    block_list->len += 2;

    return block_pair;
}

static void
remove_block(struct heap *heap, struct block_item *item, int order_n)
{
    struct block_list *block_list= &heap->block_lists[order_n];
    block_list->len--;
    memset(item, 0, sizeof(struct block_item));
}

static struct block_item*
split_block(struct heap *heap, struct block_item *item, int order_n)
{
    struct block_item *res = add_block_pair(heap, item, order_n - 1);
    if (!res) {
	return NULL;
    }
    
    remove_block(heap, item, order_n);
    return res;
}

static struct block_item*
get_block(struct heap *heap, int order_n)
{
    struct block_item *item = find_free_block(heap, order_n);
    if (!item) {
	/*
	  No free block of order_n was available.
	  Get a block of order_n + 1.
	 */
	item = get_block(heap, order_n + 1);
	if (!item) {
	    return NULL;	/* Out of memory */
	}

	/*
	  We have a block of order_n + 1, split the
	  block into two blocks of order_n.
	 */
	item = split_block(heap, item, order_n + 1);
    }

    return item;
}

static struct block_item*
find_allocated_ptr_in_list(struct block_list *block_list, void *ptr)
{
    struct block_item *item = 0;

    for (size_t i = 0; i < block_list->size; i++) {
	if (block_list->list[i].addr == ptr) {
	    item = &block_list->list[i];
	    break;
	}
    }

    return item;
}

static int
find_allocated_ptr(struct heap *heap, void *ptr, struct block_item **item)
{
    int order_n;
    struct block_item *item = 0;
    
    for (order_n = 0; i < LATTE_HEAP_MAX_ORDER + 1; order_n++) {
	struct block_list *block_list = heap->block_lists[order_n];

	/* Skip looking at this list if there are no items allocated */
	if (block_list->num_alloc == 0) {
	    continue;
	}

	/* Try to find the item in this list  */
	item = find_allocated_ptr_in_list(block_list, ptr);
	if (item) {
	    break;
	}
    }

    *item = item;
    return order_n;
}

static struct block_item*
find_buddy_block(struct heap *heap, struct block_item *item, int order_n)
{
    struct block_item *buddy = 0;
    unsigned int order_n_block_size = LATTE_HEAP_MIN_BLOCK_SIZE * (1 << order_n);
    unsigned int order_n_plus_one_block_size = LATTE_HEAP_MIN_BLOCK_SIZE * (1 << order_n + 1);
    
    if (order_n == LATTE_HEAP_MAX_ORDER) {
	return NULL;
    }
    
    unsigned int is_aligned = item->addr % order_n_plus_one_block_size;
    if (aligned) {
	buddy = item->addr + order_n_block_size;
    } else {
	buddy = item->addr - order_n_block_size;
    }

    return buddy;
}

static struct block_item*
merge_blocks(struct heap *heap, struct block_item *item, struct block_item *buddy, int order_n)
{
    /*
      Remove item and buddy
      Add new block of order_n+1 at min_addr(item, buddy)
      return new block
    */
    remove_block(heap, item, order_n);
    remove_block(heap, buddy, order_n);
}

int
heap_init(struct heap *heap, void *saddr)
{
    /* Initialize all block lists to zero  */
    memset(heap->_order_0_list, 0, sizeof(heap->_order_0_list));
    memset(heap->_order_1_list, 0, sizeof(heap->_order_1_list));
    memset(heap->_order_2_list, 0, sizeof(heap->_order_2_list));
    memset(heap->_order_3_list, 0, sizeof(heap->_order_3_list));
    memset(heap->_order_4_list, 0, sizeof(heap->_order_4_list));
    memset(heap->_order_5_list, 0, sizeof(heap->_order_5_list));
    memset(heap->_order_6_list, 0, sizeof(heap->_order_6_list));
    memset(heap->_order_7_list, 0, sizeof(heap->_order_7_list));
    memset(heap->_order_8_list, 0, sizeof(heap->_order_8_list));
    memset(heap->_order_9_list, 0, sizeof(heap->_order_9_list));
    memset(heap->_order_10_list, 0, sizeof(heap->_order_10_list));

    /* Setup block list pointers */
    heap->block_lists[0].list = heap->_order_0_list;
    heap->block_lists[0].len = 0;
    heap->block_lists[0].num_alloc = 0;
    heap->block_lists[0].size = block_list_size(0);

    heap->block_lists[1].list = heap->_order_1_list;
    heap->block_lists[1].len = 0;
    heap->block_lists[1].num_alloc = 0;
    heap->block_lists[1].size = block_list_size(1);

    heap->block_lists[2].list = heap->_order_2_list;
    heap->block_lists[2].len = 0;
    heap->block_lists[2].num_alloc = 0;
    heap->block_lists[2].size = block_list_size(2);

    heap->block_lists[3].list = heap->_order_3_list;
    heap->block_lists[3].len = 0;
    heap->block_lists[3].num_alloc = 0;
    heap->block_lists[3].size = block_list_size(3);

    heap->block_lists[4].list = heap->_order_4_list;
    heap->block_lists[4].len = 0;
    heap->block_lists[4].num_alloc = 0;
    heap->block_lists[4].size = block_list_size(4);

    heap->block_lists[5].list = heap->_order_5_list;
    heap->block_lists[5].len = 0;
    heap->block_lists[5].num_alloc = 0;
    heap->block_lists[5].size = block_list_size(5);

    heap->block_lists[6].list = heap->_order_6_list;
    heap->block_lists[6].len = 0;
    heap->block_lists[6].num_alloc = 0;
    heap->block_lists[6].size = block_list_size(6);

    heap->block_lists[7].list = heap->_order_7_list;
    heap->block_lists[7].len = 0;
    heap->block_lists[7].num_alloc = 0;
    heap->block_lists[7].size = block_list_size(7);

    heap->block_lists[8].list = heap->_order_8_list;
    heap->block_lists[8].len = 0;
    heap->block_lists[8].num_alloc = 0;
    heap->block_lists[8].size = block_list_size(8);

    heap->block_lists[9].list = heap->_order_9_list;
    heap->block_lists[9].len = 0;
    heap->block_lists[9].num_alloc = 0;
    heap->block_lists[9].size = block_list_size(9);

    heap->block_lists[10].list = heap->_order_10_list;
    heap->block_lists[10].len = 0;
    heap->block_lists[10].num_alloc = 0;
    heap->block_lists[10].size = block_list_size(10);
    
    /* Set heap start address  */
    heap->saddr = saddr;

    allocate_initial_blocks(heap);

    return 0;
}

void*
heap_malloc(struct heap *heap, size_t size)
{
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

void
heap_free(struct heap *heap, void *ptr)
{
    struct block_item *item = 0;
    int order_n = find_allocated_ptr(heap, ptr, &item);
    if (!item) {
	return;			/* Couldn't find the ptr */
    }

    item->flags = HEAP_BLOCK_AVAILABLE;

    struct block_item *buddy = find_buddy_block(heap, item, order_n);
    while (item->flags & HEAP_BLOCK_AVAILABLE
	   && buddy
	   && buddy->flags & HEAP_BLOCK_AVAILABLE) {
	item = merge_blocks(heap, item, buddy, order_n);
	buddy = find_buddy_block(heap, item, order_n);
	order_n++;
    }
}
