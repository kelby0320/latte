#include "vfs/mountpoint.h"

#include "config.h"
#include "errno.h"
#include "libk/memory.h"
#include "libk/string.h"

static struct mountpoint *mountpoint_list[LATTE_MAX_MOUNTPOINTS];
static int mountpoint_list_len;

void
mountpoint_init()
{
    memset(mountpoint_list, 0, sizeof(mountpoint_list));
    mountpoint_list_len = 0;
}

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
    for (int i = 0; i < LATTE_MAX_MOUNTPOINTS; i++) {
        struct mountpoint *mountpoint = mountpoint_list[i];
        size_t len = strlen(mountpoint->path);
        if (strncmp(mountpoint->path, filename, len) == 0) {
            return mountpoint;
        }
    }

    return NULL;
}