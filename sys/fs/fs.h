#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <stdint.h>

typedef unsigned int FILE_SEEK_MODE;

enum { SEEK_SET, SEEK_CUR, SEEK_END };

typedef unsigned int FILE_MODE;

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

struct disk;
struct path;

/**
 * @brief Filesystem interface structure
 *
 */
struct filesystem {
    // Pointer to function to resolve the filesystem
    int (*resolve)(struct disk *disk);

    // Pointer to function to open a file
    int (*open)(struct disk *disk, struct path *path, FILE_MODE mode, void **out);

    // Pointer to function to close a file
    int (*close)(void *private);

    // Pointer to function to seek in a file
    int (*seek)(struct disk *disk, void *private, uint32_t offset, FILE_SEEK_MODE seek_mode);

    // Pointer to function to read from a file
    int (*read)(struct disk *disk, void *private, char *buf, uint32_t count);

    // Pointer to function to write to a file
    int (*write)(struct disk *disk, void *private, const char *buf, uint32_t count);

    // Pointer to function to read file status
    int (*stat)(struct disk *disk, void *private, struct file_stat *stat);

    // Pointer to private fs data
    void *private;

    // Filesystem name
    char name[128];
};

/**
 * @brief Common file descriptor structure
 *
 */
struct file_descriptor {
    // Pointer to disk
    struct disk *disk;

    // Pointer to filesystem
    struct filesystem *filesystem;

    // Pointer to private descriptor data
    void *private;

    // Global file descriptor number
    int index;
};

/**
 * @brief Inialize filesystems
 *
 */
void
fs_init();

/**
 * @brief Open a file
 *
 * @param filename  Path string to the file
 * @param mode_str  Open file mode
 * @return int      Status code
 */
int
fopen(const char *filename, const char *mode_str);

/**
 * @brief Close an open file
 *
 * @param fd        Global file descriptor number
 * @return int      Status code
 */
int
fclose(int fd);

/**
 * @brief Seek to a location in an open file
 *
 * @param fd        Global file descriptor numer
 * @param offset    Seek location
 * @param whence    Seek mode
 * @return int      Status code
 */
int
fseek(int fd, int offset, FILE_SEEK_MODE whence);

/**
 * @brief Read from an open file
 *
 * @param fd        Global file descriptor number
 * @param ptr       Pointer to output buffer
 * @param count     Number of bytes to read
 * @return int      Number of bytes to read
 */
int
fread(int fd, char *ptr, size_t count);

/**
 * @brief Write to an open file
 *
 * @param fd        Global file descriptor number
 * @param ptr       Pointer to buffer
 * @param count     Number of bytes to write
 * @return int      Number of bytes actually written
 */
int
fwrite(int fd, const char *ptr, size_t count);

/**
 * @brief Read file status
 *
 * @param fd        Global file descriptor number
 * @param stat      Pointer to file_stat structure
 * @return int      Status code
 */
int
fstat(int fd, struct file_stat *stat);

/**
 * @brief Attempt to bind a filesystem to a disk
 *
 * @param disk      Pointer to the disk
 * @return struct filesystem* Bound filesystem or 0
 */
struct filesystem *
fs_resolve(struct disk *disk);

#endif