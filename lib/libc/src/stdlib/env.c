#include "stdlib.h"

char **envp;

char *
getenv(const char *name)
{
    return NULL;
}

int
putenv(char *string)
{
    return -1;
}

int
setenv(const char *name, const char *value, int overwrite)
{
    return -1;
}