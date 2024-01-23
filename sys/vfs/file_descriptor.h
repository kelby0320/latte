#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

struct mountpoint;

/**
 * @brief Common file descriptor structure
 *
 */
struct file_descriptor {
    // Pointer to the mountpoint for this file
    struct mountpoint *mountpoint;

    // Pointer to private descriptor data
    void *private;

    // Global file descriptor number
    int index;
};

/**
 * @brief Initialize global file descriptor table
 *
 */
void
file_descriptor_init();

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