#include "fs/fs.h"

#include "config.h"
#include "errno.h"
#include "dev/disk/disk.h"
#include "fs/ext2/ext2.h"
#include "fs/fat32/fat32.h"
#include "fs/path.h"
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

static struct file_descriptor*
fs_get_descriptor(int fd)
{
    if (fd < 0 || fd >= LATTE_MAX_FILE_DESCRIPTORS) {
        return 0;
    }

    return file_descriptors[fd];
}

static int
fs_get_new_descriptor(struct file_descriptor **desc_out)
{
    for (int i = 0; i < LATTE_MAX_FILE_DESCRIPTORS; i++) {
        if (file_descriptors[i] == 0) {
            struct file_descriptor *desc = kzalloc(sizeof(struct file_descriptor));
            if (!desc) {
                return -ENOMEM;
            }

            desc->index = i;
            file_descriptors[i] = desc;
            *desc_out = desc;
            return 0;
        }
    }

    return -ENFILE;
}

static void
fs_free_descriptor(struct file_descriptor *descriptor)
{
    file_descriptors[descriptor->index] = 0;
    kfree(descriptor);
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

static FILE_MODE 
fs_get_mode_from_string(const char* str)
{
    FILE_MODE mode = FILE_MODE_INVALID;

    if (strncmp(str, "r", 1) == 0) {
        mode = FILE_MODE_READ;
    } else if (strncmp(str, "w", 1) == 0) {
        mode = FILE_MODE_WRITE;
    } else if (strncmp(str, "a", 1) == 0) {
        mode = FILE_MODE_APPEND;
    }

    return mode;
}

int
fopen(const char* filename, const char* mode_str)
{
    struct file_descriptor *descriptor; 
    int res = fs_get_new_descriptor(&descriptor);
    if (res < 0) {
        return res;
    }

    struct path *path;
    path_from_str(&path, filename);
    if (!path) {
        res = -EACCES;
        goto err_out;
    }

    struct disk *disk = disk_get_disk(path->disk_id);
    if (!disk) {
        res = -EACCES;
        goto err_out;
    }

    FILE_MODE mode = fs_get_mode_from_string(mode_str);
    if (mode == FILE_MODE_INVALID) {
        res = -EINVAL;
        goto err_out;
    }

    res = disk->fs->open(disk, path, mode, &descriptor->private);
    if (res < 0) {
        goto err_out;
    }

    descriptor->disk = disk;
    descriptor->filesystem = disk->fs;

    return descriptor->index;

err_out:
    fs_free_descriptor(descriptor);
    return res;
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
fread(int fd, char* ptr, size_t count)
{
    struct file_descriptor *descriptor = fs_get_descriptor(fd);
    if (!descriptor) {
        return -EINVAL;
    }

    struct disk *disk = descriptor->disk;
    struct filesystem *fs = descriptor->filesystem;

    int res = fs->read(disk, descriptor->private, ptr, count);
    return res;
}

int
fwrite(int fd, const char *ptr, size_t count)
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