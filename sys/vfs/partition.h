#ifndef PARTITION_H
#define PARTITION_H

struct block_device;
struct filesystem;
struct partition_table_entry;

/**
 * @brief Partition structure
 *
 */
struct partition {
    // Pointer to the block device
    struct block_device *block_device;

    // LBA of the start of the partition
    unsigned int partition_start;

    // Pointer to the filesystem
    struct filesystem *filesystem;

    // Pointer to private data for the filesystem
    void *fs_private;
};

/**
 * @brief Initialize a partition structure
 *
 * @param partition     Pointer to the partition
 * @param block_device  Pointer to the block device
 * @param part_entry    Pointer to the partition entry
 * @return int          Status code
 */
int
partition_init(struct partition *partition, struct block_device *block_device,
               struct partition_table_entry *part_entry);

/**
 * @brief Bind a filesystem to a partition
 *
 * @param partition     Pointer to the partition
 * @return int          Status code
 */
int
partition_bind_fs(struct partition *partition);

#endif