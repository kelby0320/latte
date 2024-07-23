#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include <stdint.h>

struct mountpoint;

typedef unsigned int file_type_t;

enum file_type {
    FT_REGULAR_FILE,
    FT_DIRECTORY
};

/**
 * @brief Common file descriptor structure
 *
 */
struct file_descriptor {
    // File descriptor type
    file_type_t type;
    
    // Pointer to the mountpoint for this file
    struct mountpoint *mountpoint;

    // Pointer to private descriptor data
    void *private;

    // Global file descriptor number
    int index;
};

struct dir_entry {
    char name[256];
    uint32_t inode;
};

/**
 * @brief Retrieve a file descriptor
 *
 * @param fd    The file descritpor index
 * @return struct file_descriptor* Pointer to the associated descriptor or NULL
 */
struct file_descriptor *
file_descriptor_get(int fd);

/**
 * @brief Allocate a new file descriptor
 *
 * @param desc_out  Pointer to the descriptor
 * @return int      Status code
 */
int
file_descriptor_get_new(struct file_descriptor **desc_out);

/**
 * @brief Deallocate a file_descriptor
 *
 * @param descriptor The file descriptor
 */
void
file_descriptor_free(struct file_descriptor *descriptor);

#endif
