#include "fs/path.h"

#include "config.h"
#include "errno.h"
#include "libk/kheap.h"
#include "libk/memory.h"
#include "libk/string.h"

#include <stdbool.h>

/**
 * @brief Predicate for path characters
 *
 * @param c     Character to test
 * @return true
 * @return false
 */
static bool
is_path_char(char c)
{
    if (isalnum(c)) {
        return true;
    }

    if (c == '-') {
        return true;
    }

    if (c == '_') {
        return true;
    }

    if (c == '.') {
        return true;
    }

    if (c == '~') {
        return true;
    }

    return false;
}

/**
 * @brief Get the element from path object
 *
 * @param path_buf  Output buffer
 * @param path_str  Path string
 * @return int      Element length
 */
static int
get_element_from_path(char *path_buf, const char **path_str)
{
    int i;
    for (i = 0; is_path_char(**path_str) && i < LATTE_MAX_PATH_ELEM_SIZE - 1; i++) {
        path_buf[i] = **path_str;
        *path_str += 1;
    }

    path_buf[i] = '\0'; // Terminate string

    if (**path_str == PATH_SEPARATOR) {
        *path_str += 1;
    }

    return i; // Return element length
}

/**
 * @brief Parse the next element in the path
 *
 * @param next_element  Output pointer to the next path element structure
 * @param prev_element  Pointer to the prevous element
 * @param path_str      Path string
 * @return int          Element length
 */
static int
parse_next_element(struct path_element **next_element, struct path_element *prev_element,
                   const char **path_str)
{
    struct path_element *element = kzalloc(sizeof(struct path_element));
    if (!element) {
        return -ENOMEM;
    }

    int res = get_element_from_path(element->element, path_str);
    if (res == 0) {
        // No more elements in path
        kfree(element);
        return res;
    }

    // Link previous path element
    if (prev_element) {
        prev_element->next = element;
    }

    *next_element = element;
    return res;
}

int
path_from_str(struct path **path_out, const char *path_str)
{
    // The path string should start with '/'
    if (*path_str != '/') {
        return -EINVAL;
    }

    // Skip initial '/'
    path_str++;

    struct path *path = kzalloc(sizeof(struct path));
    if (!path) {
        return -ENOMEM;
    }

    struct path_element *root_element;
    int res = parse_next_element(&root_element, NULL, &path_str);
    if (res < 0) {
        goto err_out;
    }

    path->root = root_element;
    struct path_element *next_element;
    res = parse_next_element(&next_element, root_element, &path_str);
    while (res > 0) {
        res = parse_next_element(&next_element, next_element, &path_str);
    }

    *path_out = path;
    return 0;

err_out:
    path_free(path);
    return res;
}

void
path_free(struct path *path)
{
    for (struct path_element *p = path->root; p != NULL;) {
        struct path_element *temp = p->next;
        kfree(p);
        p = temp;
    }

    kfree(path);
}