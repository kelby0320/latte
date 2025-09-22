#ifndef LIBK_MEMORY_H
#define LIBK_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
int
memcmp(const void *ptr1, const void *ptr2, size_t n);

void *
memcpy(void *dest, const void *src, size_t n);

void *
memset(void *dest, int ch, size_t n);

#ifdef __cplusplus
}
#endif
#endif
