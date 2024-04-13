#include "libk/slab.h"

#include "errno.h"
#include "libk/alloc.h"
#include "libk/memory.h"

#define SLAB_ITEM_FREE 0
#define SLAB_ITEM_USED 1

/**
 * @brief Initialize a slab
 *
 * @param slab      Pointer to the slab
 * @param obj_size  Size of the objects in the slab
 * @return int      Status code
 */
static int
slab_init(struct slab *slab, size_t obj_size)
{
    slab->slab_objs = vzalloc(SLAB_SIZE);
    slab->slab_len = SLAB_SIZE / obj_size;
    slab->obj_size = obj_size;
    slab->objs_free = slab->slab_len;
    slab->objs_used = 0;
    slab->next = NULL;
    memset(slab->free_list, 0, sizeof(slab->free_list));

    return 0;
}

/**
 * @brief   Free a slab
 *
 * @param slab  Pointer to the slab
 */
static void
slab_free(struct slab *slab)
{
    vfree(slab->slab_objs);
}

/**
 * @brief   Append a slab to the end of the slab list
 *
 * @param slab_head Pointer to the head of the slab list
 * @param new_slab  Pointer to the new slab
 */
static void
slab_append_slab(struct slab *slab_head, struct slab *new_slab)
{
    struct slab *slab = slab_head;
    while (slab->next) {
        slab = slab->next;
    }

    slab->next = new_slab;
}

/**
 * @brief   Allocate an object from a slab
 *
 * @param slab      Pointer to the slab
 * @return void*    Pointer to the object
 */
static void *
slab_alloc(struct slab *slab)
{
    if (slab->objs_free == 0) {
        return NULL;
    }

    for (size_t i = 0; i < slab->slab_len; i++) {
        if (slab->free_list[i] == SLAB_ITEM_FREE) {
            slab->free_list[i] = SLAB_ITEM_USED;
            slab->objs_free--;
            slab->objs_used++;
            return slab->slab_objs + (i * slab->obj_size);
        }
    }

    return NULL;
}

/**
 * @brief   Deallocate an object from a slab
 *
 * @param slab  Pointer to the slab
 * @param ptr   Pointer to the object
 * @return int  Status code
 */
static int
slab_dealloc(struct slab *slab, void *ptr)
{
    size_t idx = (ptr - slab->slab_objs) / slab->obj_size;
    if (idx >= FREE_LIST_SIZE) {
        return -ENOENT;
    }

    slab->free_list[idx] = SLAB_ITEM_FREE;
    slab->objs_free++;
    slab->objs_used--;

    return 0;
}

int
slab_cache_create(struct slab_cache *cache, size_t obj_size)
{
    cache->obj_size = obj_size;
    cache->slab_head = vzalloc(sizeof(struct slab));
    slab_init(cache->slab_head, obj_size);

    return 0;
}

void
slab_cache_destroy(struct slab_cache *cache)
{
    for (struct slab *slab = cache->slab_head; slab;) {
        struct slab *next = slab->next;
        slab_free(slab);
        vfree(slab);
        slab = next;
    }
}

void *
slab_cache_alloc(struct slab_cache *cache)
{
    for (struct slab *slab = cache->slab_head; slab; slab = slab->next) {
        void *ptr = slab_alloc(slab);
        if (ptr) {
            return ptr;
        }
    }

    struct slab *new_slab = vzalloc(sizeof(struct slab));
    slab_init(new_slab, cache->obj_size);
    slab_append_slab(cache->slab_head, new_slab);

    return slab_alloc(new_slab);
}

int
slab_cache_free(struct slab_cache *cache, void *ptr)
{
    for (struct slab *slab = cache->slab_head; slab; slab = slab->next) {
        int res = slab_dealloc(slab, ptr);
        if (res == 0) {
            return res;
        }
    }

    return -ENOENT;
}