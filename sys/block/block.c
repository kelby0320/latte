#include "block.h"

#include "libk/list.h"

#include <stddef.h>

static struct list_item *block_list = NULL;

int
block_add(struct block *block)
{
    return list_push_back(&block_list, block);
}

struct block *
block_find(bool (*predicate)(struct block *))
{
    for_each_in_list(struct block *, block_list, list, block) {
	if (predicate(block)) {
	    return block;
	}
    }

    return NULL;
}
