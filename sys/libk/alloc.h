#ifndef LIBK_ALLOC_H
#define LIBK_ALLOC_H

#include <stddef.h>

void
libk_alloc_init();

void *
vmalloc(size_t size);

void *
vzalloc(size_t size);

void
vfree();

void *
kmalloc(size_t size);

void *
kzalloc(size_t size);

void
kfree(void *ptr);

#endif