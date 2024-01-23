#ifndef BUFFEREDREADER_H
#define BUFFEREDREADER_H

#include <stddef.h>

struct block_device;

/**
 * @brief Data buffer for reading from a disk.
 *
 * This object enables reading arbitrary amounts of data from a disk rather than a multiple of the
 * disk sector size.
 *
 */
struct bufferedreader {
    // Pointer to the disk
    struct block_device *block_device;

    // Read position on the disk
    unsigned int pos;
};

/**
 * @brief Create a new bufferedreader.
 *
 * @param reader_out    Pointer to struct that will be allocated and initialized.
 * @param block_device  Pointer to the block device
 * @return int          Status code
 */
int
bufferedreader_new(struct bufferedreader **reader_out, struct block_device *block_device);

/**
 * @brief Seek to a position on the disk
 *
 * @param reader    Pointer to the bufferedreader
 * @param pos       Position to seek to
 * @return int      Status code
 */
int
bufferedreader_seek(struct bufferedreader *reader, unsigned int pos);

/**
 * @brief Read an arbitrary amount of data
 *
 * @param reader    Pointer to the bufferedreader
 * @param out       Output buffer
 * @param count     Amount of bytes to read
 * @return int      Number of bytes actually read
 */
int
bufferedreader_read(struct bufferedreader *reader, void *out, int count);

/**
 * @brief Free the resources associated with a bufferedreader
 *
 * @param reader    Pointer to the bufferedreader
 */
void
bufferedreader_free(struct bufferedreader *reader);

#endif