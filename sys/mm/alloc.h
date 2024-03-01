#ifndef ALLOC_H
#define ALLOC_H

int
alloc_init();

void *
alloc_get_phys_page();

void
alloc_free_phys_page(void *paddr);

#endif