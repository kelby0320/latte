#ifndef PATH_H
#define PATH_H

#include "config.h"

#define PATH_SEPARATOR '/'
#define DISK_ID_SEPARATOR ':'

struct path_element {
    char element[LATTE_MAX_PATH_ELEM_SIZE];
    struct path_element *next;
};

struct path {
    char disk_id[LATTE_DISK_ID_MAX_SIZE];
    struct path_element *root;
};

int
path_from_str(struct path **path_out, const char *path_str);

void
path_free(struct path *path);

#endif