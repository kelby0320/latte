#include "mem/heap.h"
#include "libk/memory.h"


static void
heap_allocate_initial_blocks(struct heap *heap)
{
    /* Allocate all 4Mb blocks in the order 10 list */
    struct block_list *order_10_list = &heap->block_lists[10];
    unsigned int offset = LATTE_HEAP_MIN_BLOCK_SIZE * (1 << LATTE_HEAP_MAX_ORDER);

    for (size_t i = 0; i < order_10_list->size; i++) {
	order_10_list->list[i].addr = (void*)(heap->saddr + (i * offset));
	order_10_list->list[i].flag = HEAP_BLOCK_AVAILABLE;
    }
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
    heap->block_lists[0].cur = 0;
    heap->block_lists[0].size = block_list_size(0);

    heap->block_lists[1].list = heap->_order_1_list;
    heap->block_lists[1].cur = 0;
    heap->block_lists[1].size = block_list_size(1);

    heap->block_lists[2].list = heap->_order_2_list;
    heap->block_lists[2].cur = 0;
    heap->block_lists[2].size = block_list_size(2);

    heap->block_lists[3].list = heap->_order_3_list;
    heap->block_lists[3].cur = 0;
    heap->block_lists[3].size = block_list_size(3);

    heap->block_lists[4].list = heap->_order_4_list;
    heap->block_lists[4].cur = 0;
    heap->block_lists[4].size = block_list_size(4);

    heap->block_lists[5].list = heap->_order_5_list;
    heap->block_lists[5].cur = 0;
    heap->block_lists[5].size = block_list_size(5);

    heap->block_lists[6].list = heap->_order_6_list;
    heap->block_lists[6].cur = 0;
    heap->block_lists[6].size = block_list_size(6);

    heap->block_lists[7].list = heap->_order_7_list;
    heap->block_lists[7].cur = 0;
    heap->block_lists[7].size = block_list_size(7);

    heap->block_lists[8].list = heap->_order_8_list;
    heap->block_lists[8].cur = 0;
    heap->block_lists[8].size = block_list_size(8);

    heap->block_lists[9].list = heap->_order_9_list;
    heap->block_lists[9].cur = 0;
    heap->block_lists[9].size = block_list_size(9);

    heap->block_lists[10].list = heap->_order_10_list;
    heap->block_lists[10].cur = 0;
    heap->block_lists[10].size = block_list_size(10);
    
    /* Set heap start address  */
    heap->saddr = saddr;

    heap_allocate_initial_blocks(heap);

    return 0;
}


