#ifndef SLAB_H
#define SLAB_H

#include "mm/paging/paging.h"

#include <stddef.h>

// The size of the free list is determined by assuming a 4k allocation for the struct slab
// minus the sizes of the other fields in the struct slab
#define FREE_LIST_SIZE                                                                             \
    (PAGING_PAGE_SIZE - sizeof(void *) - (3 * sizeof(size_t)) - sizeof(struct slab *))

/**
 * @brief
 *
 */
struct slab {
    void *slab_addr;
    size_t slab_size;
    size_t objs_used;
    size_t objs_free;
    struct slab *next;
    unsigned int free_list[FREE_LIST_SIZE];
};

/**
 * @brief
 *
 */
struct slab_cache {
    size_t obj_size;
    struct slab *slab_head;
};

int
slab_cache_create(struct slab_cache *cache, size_t obj_size);

void
slab_cache_destroy(struct slab_cache *cache);

void *
slab_cache_alloc(struct slab_cache *cache);

void
slab_cache_free(struct slab_cache *cache, void *ptr);

#endif