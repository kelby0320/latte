#include "mm/slab.h"

#include "mm/buddy.h"
#include "mm/kalloc.h"
#include "mm/vm.h"

int
slab_cache_create(struct slab_cache *cache, size_t obj_size)
{
}

void
slab_cache_destroy(struct slab_cache *cache)
{
}

void *
slab_cache_alloc(struct slab_cache *cache)
{
}

void
slab_cache_free(struct slab_cache *cache, void *ptr)
{
}