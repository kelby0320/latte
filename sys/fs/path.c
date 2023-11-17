#include "fs/path.h"

#include "config.h"
#include "errno.h"
#include "libk/kheap.h"
#include "libk/memory.h"
#include "libk/string.h"

#include <stdbool.h>

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


static int
validate_disk_id(const char *str)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        if (!is_path_char(str[i])) {
            return -EINVAL;
        }
    }

    return 0;
}

static int
validate_disk_id_sep(const char *str)
{
    if (str[0] != ':') {
        return -EINVAL;
    }

    if (str[1] != '/') {
        return -EINVAL;
    }

    return 0;
}

static int
disk_id_end_idx(const char *path_str)
{
    for (int idx = 0; idx < LATTE_DISK_ID_MAX_SIZE; idx++) {
        if (path_str[idx] == DISK_ID_SEPARATOR) {
            return idx - 1; // return end of disk_id index
        }
    }

    return -EINVAL;
}

static int
parse_disk_id(char *buf, const char **path_str)
{
    int end_disk_id = disk_id_end_idx(*path_str);
    if (end_disk_id < 0) {
        return -EINVAL;
    }

    int disk_id_strlen = end_disk_id + 1;
    strncpy(buf, *path_str, disk_id_strlen);
    buf[disk_id_strlen] = 0; // Ensure string is null terminated

    int res = validate_disk_id(buf);
    if (res < 0) {
        return -EINVAL;
    }
    

    *path_str += end_disk_id + 1; // point to drive_id separator ":/"
    res = validate_disk_id_sep(*path_str);
    if (res < 0) {
        return -EINVAL;
    }

    *path_str += 2; // point to next character after the drive_id separator

    return 0;
}

static int
get_element_from_path(char *path_buf, char **path_str)
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

static int
parse_next_element(struct path_element **next_element, struct path_element *prev_element, char **path_str)
{
    struct path_element *element = kzalloc(sizeof(struct path_element));
    if (!element) {
        return -ENOMEM;
    }

    int res = get_element_from_path(&element->element, path_str);
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
    struct path *path = kzalloc(sizeof(struct path));
    if (!path) {
        return -ENOMEM;
    }

    int res = parse_disk_id(path->disk_id, &path_str);
    if (res < 0) {
        goto err_out;
    }

    struct path_element *root_element;
    res = parse_next_element(&root_element, NULL, &path_str);
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
    // TODO
}