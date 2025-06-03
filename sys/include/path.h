#ifndef PATH_H
#define PATH_H

#include "config.h"

#define PATH_SEPARATOR    '/'
#define DISK_ID_SEPARATOR ':'

/**
 * @brief Linked list of elements in a path
 *
 * e.g:
 *  For a file path such as - /home/user/file.txt
 *  The path_element linked list would be as follows
 *
 *  0x1000 - {element = "home", next = 0x1044}
 *  0x1044 - {element = "user", next = 0x1088}
 *  0x1088 - {element = "file.txt", next = 0x0000}
 */
struct path_element {
    // Element name
    char element[LATTE_MAX_PATH_ELEM_SIZE];

    // Pointer to the next element
    struct path_element *next;
};

/**
 * @brief Object representing a path
 *
 */
struct path {
    // Linked list of path elements
    struct path_element *root;
};

/**
 * @brief Make a path from a path string
 *
 * @param path_out  Output pointer to path structure
 * @param path_str  Path string
 * @return int      Status code
 */
int
path_from_str(struct path **path_out, const char *path_str);

/**
 * @brief Free a path object
 *
 * @param path  Pointer to path
 */
void
path_free(struct path *path);

#endif