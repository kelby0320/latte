#ifndef VFS_H
#define VFS_H

#include "fs/fs.h"

#include <stddef.h>
#include <stdint.h>

struct block;

/**
 * @brief Initialize Virtual File System
 *
 * @return int  Status code
 */
int
vfs_init();

/**
 * @brief Mount a partition
 *
 * @param path          Path to mount in the VFS
 * @param block_device  Pointer to the block
 * @return int          Status code
 */
int
vfs_mount(const char *path, struct block *block);

/**
 * @brief Open a file
 *
 * @param filename  Name of the file
 * @param mode_str  File open mode
 * @return int      File descriptor
 */
int
vfs_open(const char *filename, const char *mode_str);

/**
 * @brief Close an open file
 *
 * @param fd    Global file descriptor number
 * @return int  Status code
 */
int
vfs_close(int fd);

/**
 * @brief Read an open file
 *
 * @param fd    Global file descriptor number
 * @param ptr   Buffer to read into
 * @param count Number of bytes to read
 * @return int  Number of bytes read
 */
int
vfs_read(int fd, char *ptr, size_t count);

/**
 * @brief Write to and open file
 *
 * @param fd    Global file descriptor number
 * @param ptr   Buffer to write to the file
 * @param count Number of bytes to write
 * @return int  Number of bytes written
 */
int
vfs_write(int fd, const char *ptr, size_t count);

/**
 * @brief Stat an open file
 *
 * @param fd    Global file descriptor number
 * @param stat  Pointer to the stat structure to fill
 * @return int  Status code
 */
int
vfs_stat(int fd, struct file_stat *stat);

/**
 * @brief Seek in an open file
 *
 * @param fd        Global file descriptor number
 * @param offset    Seek offset
 * @param whence    Seek mode
 * @return int      Status code
 */
int
vfs_seek(int fd, int offset, file_seek_mode_t whence);

#endif
