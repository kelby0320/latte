#include "libk/alloc.h"

#include "errno.h"
#include "kernel.h"
#include "libk/memory.h"
#include "libk/slab.h"
#include "mm/kalloc.h"
#include "mm/vm.h"

#define KMALLOC_SLAB_CACHE_MIN_SIZE    32
#define KMALLOC_SLAB_CACHE_MAX_SIZE    2048
#define KMALLOC_SLAB_CACHES            7
#define KMALLOC_IDX_TO_CACHE_SIZE(idx) (KMALLOC_SLAB_CACHE_MIN_SIZE << idx)

static struct slab_cache kmalloc_slab_cache[KMALLOC_SLAB_CACHES];

extern struct vm_area kernel_vm_area;

/**
 * @brief   Converts a size to the number of extents required to allocate that size
 *
 * @param size  Size to convert
 * @return int  Number of extents required
 */
static int
size_to_max_order_extents(size_t size)
{
    return (size + BUDDY_BLOCK_MAX_SIZE) / BUDDY_BLOCK_MAX_SIZE;
}

/**
 * @brief   Converts a size to the index of the slab cache to use
 *
 * @param size  Size to convert
 * @return int  Index of the slab cache
 */
static int
size_to_cache_idx(size_t size)
{
    int idx = 0;
    size_t slab_size = KMALLOC_SLAB_CACHE_MIN_SIZE;
    while ((size / slab_size) != 0) {
        idx++;
        slab_size <<= 1;
    }

    return idx;
}

static void *
vmalloc_large_pages(size_t size)
{
    if (size < BUDDY_BLOCK_MAX_SIZE) {
        return NULL;
    }

    int num_extents = size_to_max_order_extents(size);

    // The max number of extents for a 4GB allocation is 1024
    // This puts the upper bound at 4k of stack space for this array
    void *phys_mem[num_extents];
    memset(phys_mem, 0, sizeof(phys_mem));

    for (int i = 0; i < num_extents; i++) {
        void *phys = kalloc_get_phys_pages(BUDDY_BLOCK_MAX_ORDER);
        if (!phys) {
            goto err_out;
        }
        phys_mem[i] = phys;

        if (i > 1) {
            kalloc_link_contiguous_allocation(phys_mem[i - 1], phys_mem[i]);
        }
    }

    void *vaddr = vm_area_map_kernel_large_pages(phys_mem, num_extents);
    if (!vaddr) {
        goto err_out;
    }

    return vaddr;

err_out:
    for (int i = 0; i < num_extents; i++) {
        if (phys_mem[i]) {
            kalloc_free_phys_pages(phys_mem[i]);
        }
    }

    return NULL;
}

void
libk_alloc_init()
{
    for (int i = 0; i < KMALLOC_SLAB_CACHES; i++) {
        slab_cache_create(&kmalloc_slab_cache[i], KMALLOC_IDX_TO_CACHE_SIZE(i));
    }
}

void *
vmalloc(size_t size)
{
    if (size <= KMALLOC_SLAB_CACHE_MAX_SIZE) {
        return kmalloc(size);
    }

    if (size > BUDDY_BLOCK_MAX_SIZE) {
        return vmalloc_large_pages(size);
    }

    int order = kalloc_size_to_order(size);
    if (order < 0) {
        return NULL;
    }

    void *phys = kalloc_get_phys_pages(order);
    if (!phys) {
        goto err_out;
    }

    size_t mem_size = kalloc_order_to_size(order);

    void *vaddr = vm_area_map_kernel_pages(phys, mem_size);
    if (!vaddr) {
        goto err_out;
    }

    return vaddr;

err_out:
    kalloc_free_phys_pages(phys);
    return NULL;
}

void *
vzalloc(size_t size)
{
    if (size <= KMALLOC_SLAB_CACHE_MAX_SIZE) {
        return kzalloc(size);
    }

    void *vaddr = vmalloc(size);
    if (!vaddr) {
        return NULL;
    }

    // We need the actual allocation size, not the requested size
    int order = kalloc_size_to_order(size);
    size_t mem_size = kalloc_order_to_size(order);

    memset(vaddr, 0, mem_size);

    return vaddr;
}

void
vfree(void *ptr)
{
    void *paddr = vm_area_virt_to_phys(&kernel_vm_area, ptr);
    if (!paddr) {
        return;
    }

    while (paddr) {
        kalloc_free_phys_pages(paddr);
        void *next_paddr = kalloc_get_next_contiguous_allocation(paddr);
        kalloc_unlink_contiguous_allocation(paddr, next_paddr);
        paddr = next_paddr;
    }
}

void *
kmalloc(size_t size)
{
    if (size > KMALLOC_SLAB_CACHE_MAX_SIZE) {
        return vmalloc(size);
    }

    int idx = size_to_cache_idx(size);

    return slab_cache_alloc(&kmalloc_slab_cache[idx]);
}

void *
kzalloc(size_t size)
{
    if (size > KMALLOC_SLAB_CACHE_MAX_SIZE) {
        return vzalloc(size);
    }

    int idx = size_to_cache_idx(size);

    void *ptr = slab_cache_alloc(&kmalloc_slab_cache[idx]);

    size_t slab_size = KMALLOC_IDX_TO_CACHE_SIZE(idx);
    memset(ptr, 0, slab_size);

    return ptr;
}

void
kfree(void *ptr)
{
    for (int i = 0; i < KMALLOC_SLAB_CACHES; i++) {
        int res = slab_cache_free(&kmalloc_slab_cache[i], ptr);
        if (res == 0) {
            return;
        }
    }
}
