#include "string.h"

void*
memset(void *dest, int ch, size_t count)
{
    char *ptr = (char*)dest;
    for (size_t i = 0; i < count; i++) {
	ptr[i] = (char)ch;
    }

    return dest;
}
