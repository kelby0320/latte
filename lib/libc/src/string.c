#include "string.h"

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

char *
strcat(char *dest, const char *src)
{
    size_t srclen = strlen(src);
    size_t i = strlen(dest);
    size_t j = 0;

    for (; j < srclen; i++, j++) {
        dest[i] = src[j];
    }

    dest[i] = '\0';

    return dest;
}

int
strcmp(const char *str1, const char *str2)
{
    int str1_idx = 0, str2_idx = 0;

    /* Loop through both strings comparing each character */
    while (str1[str1_idx] != 0 && str2[str2_idx] != 0) {
        if (str1[str1_idx] < str2[str2_idx]) {
            return -1;
        }

        if (str1[str1_idx] > str2[str2_idx]) {
            return 1;
        }

        str1_idx++;
        str2_idx++;
    }

    /* If one string is longer than the other, check the final characters */
    if (str1[str1_idx] != 0 || str2[str2_idx] != 0) {
        if (str1[str1_idx] < str2[str2_idx]) {
            return -1;
        }

        if (str1[str1_idx] > str2[str2_idx]) {
            return 1;
        }
    }

    return 0;
}

char *
strcpy(char *dest, const char *src)
{
    size_t srclen = strlen(src) + 1; // Include null terminator

    for (size_t i = 0; i < srclen; i++) {
        dest[i] = src[i];
    }

    return dest;
}

size_t
strlen(const char *str)
{
    size_t len = 0;

    while (*str) {
        len++;
        str++;
    }

    return len;
}

char *
strncat(char *dest, const char *src, size_t n)
{
    size_t srclen = strlen(src);
    size_t limit = srclen < n ? srclen : n;
    size_t i = strlen(dest);
    size_t j = 0;

    for (; j < limit; i++, j++) {
        dest[i] = src[j];
    }

    dest[i] = '\0';

    return dest;
}

int
strncmp(const char *str1, const char *str2, size_t n)
{
    size_t str1_idx = 0, str2_idx = 0;

    /* Loop through both strings comparing each character */
    while (str1[str1_idx] != 0 && str2[str2_idx] != 0 && str1_idx < n) {
        if (str1[str1_idx] < str2[str2_idx]) {
            return -1;
        }

        if (str1[str1_idx] > str2[str2_idx]) {
            return 1;
        }

        str1_idx++;
        str2_idx++;
    }

    /* If one string is longer than the other, check the final characters */
    if ((str1[str1_idx] != 0 || str2[str2_idx] != 0) && str1_idx < n) {
        if (str1[str1_idx] < str2[str2_idx]) {
            return -1;
        }

        if (str1[str1_idx] > str2[str2_idx]) {
            return 1;
        }
    }

    return 0;
}

char *
strncpy(char *dest, const char *src, size_t n)
{
    size_t srclen = strlen(src);
    size_t limit = srclen < n ? srclen : n;
    size_t i;

    /* Copy src to dest */
    for (i = 0; i < limit; i++) {
        dest[i] = src[i];
    }

    /* Add padding if necessary */
    if (limit < n) {
        for (; i < n; i++) {
            dest[i] = 0;
        }
    }

    return dest;
}
