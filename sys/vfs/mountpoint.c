#include "vfs/mountpoint.h"

#include "config.h"
#include "errno.h"
#include "libk/memory.h"
#include "libk/string.h"

static struct mountpoint *mountpoint_list[LATTE_MAX_MOUNTPOINTS] = {0};
static int mountpoint_list_len = 0;

int
mountpoint_add(struct mountpoint *mountpoint)
{
    if (mountpoint_list_len == LATTE_MAX_MOUNTPOINTS) {
        return -EAGAIN;
    }

    mountpoint_list[mountpoint_list_len] = mountpoint;
    mountpoint_list_len++;

    return 0;
}

struct mountpoint *
mountpoint_find(const char *filename)
{
    // Hacky reverse loop will work for now, but should really be DFS
    for (int i = mountpoint_list_len - 1; i >= 0; i--) {
        struct mountpoint *mountpoint = mountpoint_list[i];
        size_t len = strlen(mountpoint->path);
        if (strncmp(mountpoint->path, filename, len) == 0) {
            return mountpoint;
        }
    }

    return NULL;
}