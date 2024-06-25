#include "block/block.h"

#include "libk/list.h"

#include <stddef.h>

static unsigned int next_id = 0;
static struct list_item *block_list = NULL;

int
block_add(struct block *block)
{
    block->id = next_id++;
    return list_push_back(&block_list, block);
}
