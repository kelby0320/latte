#ifndef SLAB_H
#define SLAB_H

#include <stddef.h>
#include <stdint.h>

#define SLAB_SIZE 4096

// The size of the free list is determined by subtracting the sizes of the
// other fields in the struct slab from the SLAB_SIZE, e.g. 4096
#define FREE_LIST_SIZE                                                         \
    (SLAB_SIZE - sizeof(void *) - (4 * sizeof(size_t)) - sizeof(struct slab *))

/**
 * @brief   The slab structure
 *
 */
struct slab {
    void *slab_objs;
    size_t slab_len;
    size_t obj_size;
    size_t objs_used;
    size_t objs_free;
    struct slab *next;
    uint8_t free_list[FREE_LIST_SIZE];
};

/**
 * @brief The slab cache structure
 *
 */
struct slab_cache {
    size_t obj_size;
    struct slab *slab_head;
};

/**
 * @brief   Create a new slab cache
 *
 * @param cache     Pointer to the slab cache
 * @param obj_size  The size of the objects in the cache
 * @return int      Status code
 */
int
slab_cache_create(struct slab_cache *cache, size_t obj_size);

/**
 * @brief   Destroy a slab cache
 *
 * @param cache     Pointer to the slab cache
 */
void
slab_cache_destroy(struct slab_cache *cache);

/**
 * @brief   Allocate an object from the slab cache
 *
 * @param cache     Pointer to the slab cache
 * @return void*    Pointer to the object
 */
void *
slab_cache_alloc(struct slab_cache *cache);

/**
 * @brief   Free an object from the slab cache
 *
 * @param cache     Pointer to the slab cache
 * @param ptr       Pointer to the object
 */
int
slab_cache_free(struct slab_cache *cache, void *ptr);

#endif