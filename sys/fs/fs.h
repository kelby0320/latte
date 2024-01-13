#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <stdint.h>

#define FILESYSTEM_NAME_MAX_LEN 128

typedef unsigned int file_seek_mode_t;

enum { SEEK_SET, SEEK_CUR, SEEK_END };

typedef unsigned int file_mode_t;

enum { FILE_MODE_READ, FILE_MODE_WRITE, FILE_MODE_APPEND, FILE_MODE_INVALID };

/**
 * @brief File status structure
 *
 */
struct file_stat {
    // File mode
    uint8_t mode;

    // File size
    uint32_t size;
};

struct path;
struct file_descriptor;
struct partition;

/**
 * @brief Filesystem interface structure
 *
 */
struct filesystem {
    // Pointer to function to resolve the filesystem
    struct filesystem *(*resolve)(const struct partition *partition);

    // Pointer to function to open a file
    int (*open)(const struct partition *partition, struct path *path, file_mode_t mode, void **out);

    // Pointer to function to close a file
    int (*close)(void *private);

    // Pointer to function to seek in a file
    int (*seek)(struct file_descriptor *file_descriptor, uint32_t offset,
                file_seek_mode_t seek_mode);

    // Pointer to function to read from a file
    int (*read)(struct file_descriptor *file_descriptor, char *buf, uint32_t count);

    // Pointer to function to write to a file
    int (*write)(struct file_descriptor *file_descriptor, const char *buf, uint32_t count);

    // Pointer to function to read file status
    int (*stat)(struct file_descriptor *file_descriptor, struct file_stat *stat);

    // Filesystem name
    char name[FILESYSTEM_NAME_MAX_LEN];
};

/**
 * @brief Initialize the fileystem subsystem
 *
 */
void
fs_init();

/**
 * @brief Resolve a filesystem to a partition
 *
 * @param partition     Pointer to the partition
 * @return int          Status code
 */
int
fs_resolve(struct partition *partition);

/**
 * @brief Convert string to FILE_MODE
 *
 * @param str   The string to parse
 * @return FILE_MODE File mode identifier
 */
file_mode_t
fs_get_mode_from_string(const char *str);

#endif