#include "fs/fs.h"

#include "config.h"
#include "fs/ext2/ext2.h"
#include "fs/fat32/fat32.h"
#include "kernel.h"
#include "libk/memory.h"

struct filesystem *filesystems[LATTE_MAX_FILESYSTEMS];
struct file_descriptor *file_descriptors[LATTE_MAX_FILE_DESCRIPTORS];

static struct filesystem*
fs_get_free_filesystem()
{
    for (int i = 0; i < LATTE_MAX_FILESYSTEMS; i++) {
        if (filesystems[i] == 0) {
            return &filesystems[i];
        }
    }

    return 0;
}

static void
fs_insert_filesystem(struct filesystem *filesystem)
{
    struct filesystem **fs = fs_get_free_filesystem();
    if (!fs) {
        panic("Unable to insert filesystem");
    }

    *fs = filesystem;
}



static void
fs_load()
{
    // fs_insert_filesystem(fat32_init());
    fs_insert_filesystem(ext2_init());
}

void
fs_init()
{
    memset(filesystems, 0, sizeof(filesystems));
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

int
fopen(const char* filename, const char* mode_str)
{
    // TODO
    return -1;
}

int
fclose(int fd)
{
    // TODO
    return -1;
}

int
fseek(int fd, int offset, FILE_SEEK_MODE whence)
{
    // TODO
    return -1;
}

int
fread(int fd, void* ptr, size_t size, size_t count)
{
    // TODO
    return -1;
}

int
fwrite(int fd, const void *ptr, size_t size, size_t count)
{
    // TODO
    return -1;
}

int
fstat(int fd, struct file_stat* stat)
{
    // TODO
    return -1;
}

struct filesystem*
fs_resolve(struct disk* disk)
{
    struct filesystem *fs = 0;
    for (int i = 0; i < LATTE_MAX_FILESYSTEMS; i++) {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0) {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}