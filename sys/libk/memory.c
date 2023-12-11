#include "libk/memory.h"

#include <stdint.h>

int
memcmp(const void *ptr1, const void *ptr2, size_t n)
{
    char *cptr1 = (char *)ptr1;
    char *cptr2 = (char *)ptr2;

    for (size_t i = 0; i < n; i++) {
        if (cptr1[i] < cptr2[i]) {
            return -1;
        }

        if (cptr1[i] > cptr2[i]) {
            return 1;
        }
    }

    return 0;
}

void *
memcpy(void *dest, const void *src, size_t n)
{
    char *dptr = (char *)dest;
    char *sptr = (char *)src;

    for (size_t i = 0; i < n; i++) {
        dptr[i] = sptr[i];
    }

    return dest;
}

void *
memset(void *dest, int ch, size_t n)
{
    char *ptr = (char *)dest;
    for (size_t i = 0; i < n; i++) {
        ptr[i] = (char)ch;
    }

    return dest;
}
