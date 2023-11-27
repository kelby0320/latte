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

/**
 * @brief File status structure
 * 
 */
struct file_stat {
    // File status flags
    FILE_STAT_FLAGS flags;

    // File size
    uint32_t filesize;
};

struct disk;
struct path;

/**
 * @brief Prototype definition of the fuction used to open a file.
 * 
 * @param disk  Pointer to the disk
 * @param path  Pointer to the path
 * @param mode  Open file mode
 * @param out   Pointer to fs specific file descriptor
 * @returns int Status code 
 * 
 */
typedef int (*FS_OPEN_FUNCTION)(struct disk *disk, struct path *path, FILE_MODE mode, void **out);

/**
 * @brief Prototype definition of the function used to close an open file.
 * 
 * @param private   Pointer to fs private data
 * @returns int     Status code
 * 
 */
typedef int (*FS_CLOSE_FUNCTION)(void *private);

/**
 * @brief Prototype defintion of the fuction used to read from a file.
 * 
 * @param disk      Pointer to the disk
 * @param private   Pointer to private fs data
 * @param buf       Output buffer
 * @param count     Number of bytes to read
 * @returns int     Number of bytes actually read
 * 
 */
typedef int (*FS_READ_FUNCTION)(struct disk *disk, void *private, char *buf, uint32_t count);

/**
 * @brief Prototype definition of the fuction to write to a file.
 * 
 * @param disk      Pointer to the disk
 * @param private   Pointer to private fs data
 * @param buf       Pointer to data buffer
 * @param count     Number of bytes to write from the data buffer
 * @returns         Number of bytes actually written
 * 
 */
typedef int (*FS_WRITE_FUNCTION)(struct disk *disk, void *private, const char *buf, uint32_t count);

/**
 * @brief Prototype definition of the function to seek to a location in a file.
 * 
 * @param private   Pointer to private fs data
 * @param offset    Location to seek to
 * @param seek_mode Seek mode
 * @returns int     Status code
 * 
 */
typedef int (*FS_SEEK_FUNCTION)(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode);

/**
 * @brief Prototype definition of the function to read file status.
 * 
 * @param disk      Pointer to the disk
 * @param private   Pointer to private fs data
 * @param stat      Pointer to file_stat structure to populate
 * @returns int     Status code
 * 
 */
typedef int (*FS_STAT_FUNCTION)(struct disk *disk, void *private, struct file_stat *stat);

/**
 * @brief Prototype definition of the function that binds a filesystem to a disk.
 * 
 * @param disk      Pointer to the disk
 * @returns int     Status code
 * 
 */
typedef int (*FS_RESOLVE_FUNCTION)(struct disk *disk);

/**
 * @brief Filesystem interface structure
 * 
 */
struct filesystem {
    // Pointer to function to resolve the filesystem
    FS_RESOLVE_FUNCTION resolve;

    // Pointer to function to open a file
    FS_OPEN_FUNCTION    open;

    // Pointer to function to close a file
    FS_CLOSE_FUNCTION   close;

    // Pointer to function to seek in a file
    FS_SEEK_FUNCTION    seek;

    // Pointer to function to read from a file
    FS_READ_FUNCTION    read;

    // Pointer to function to write to a file
    FS_WRITE_FUNCTION   write;

    // Pointer to function to read file status
    FS_STAT_FUNCTION    stat;

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
fopen(const char* filename, const char* mode_str);

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
fread(int fd, char* ptr, size_t count);

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
fstat(int fd, struct file_stat* stat);

/**
 * @brief Attempt to bind a filesystem to a disk
 * 
 * @param disk      Pointer to the disk
 * @return struct filesystem* Bound filesystem or 0
 */
struct filesystem*
fs_resolve(struct disk* disk);

#endif