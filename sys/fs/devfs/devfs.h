#ifndef DEVFS_H
#define DEVFS_H

struct device;
struct file_operations;
struct filesystem;

/**
 * @brief Initialize a devfs filesystem
 *
 * @return struct filesystem*   Pointer to the filesystem
 */
struct filesystem *
devfs_init();

/**
 * @brief Add a new node to devfs
 *
 * @param device    Pointer to the device
 * @param fops      Pointer to file_operations
 * @return int      Status code
 */
int
devfs_make_node(struct device *device, struct file_operations *fops);

#endif
