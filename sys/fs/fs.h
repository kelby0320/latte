#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <stdint.h>

typedef unsigned int FILE_SEEK_MODE;

enum {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef unsigned int FILE_MODE;

enum {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

typedef unsigned int FILE_STAT_FLAGS;

enum {
    FILE_STAT_READ_ONLY = 0b00000001
};

struct file_stat {
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

struct disk;
struct path;

typedef void*(*FS_OPEN_FUNCTION)(struct disk *disk, struct path *path, FILE_MODE mode);
typedef int (*FS_CLOSE_FUNCTION)(void *private);
typedef int (*FS_READ_FUNCTION)(struct disk *disk, void *private, uint32_t size, uint32_t count, char *buf);
typedef int (*FS_WRITE_FUNCTION)(struct disk *disk, void *private, uint32_t size, uint32_t count, const char *buf);
typedef int (*FS_SEEK_FUNCTION)(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode);
typedef int (*FS_STAT_FUNCTION)(struct disk *disk, void *private, struct file_stat *stat);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk *disk);

struct filesystem {
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION    open;
    FS_CLOSE_FUNCTION   close;
    FS_SEEK_FUNCTION    seek;
    FS_READ_FUNCTION    read;
    FS_WRITE_FUNCTION   write;
    FS_STAT_FUNCTION    stat;

    char name[128];
};

struct file_descriptor {
    struct disk *disk;
    struct filesystem *filesystem;
    void *private;
};

void
fs_init();

int
fopen(const char* filename, const char* mode_str);

int
fclose(int fd);

int
fseek(int fd, int offset, FILE_SEEK_MODE whence);

int
fread(int fd, void* ptr, size_t size, size_t count);

int
fwrite(int fd, const void *ptr, size_t size, size_t count);

int
fstat(int fd, struct file_stat* stat);

struct filesystem*
fs_resolve(struct disk* disk);

#endif