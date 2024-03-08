#ifndef ALLOC_H
#define ALLOC_H

#include "mm/alloc/buddy.h"

#include <stddef.h>

#define MAX_ALLOCATABLE_MEMORY 0x100000000 // 4GB
#define MAX_ALLOCATORS (MAX_ALLOCATABLE_MEMORY / BUDDY_BLOCK_MAX_SIZE)
#define PHYS_PAGE_SIZE 4096
#define PHYS_PAGE_ORDER 0

void
alloc_init(void *saddr, size_t mem_size);

void *
alloc_get_phys_page();

void
alloc_free_phys_page(void *paddr);

#endif