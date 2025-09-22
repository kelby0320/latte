#ifndef EXT2_H
#define EXT2_H

struct filesystem;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Initialize an Ext2 filesystem
 *
 * @return struct filesystem* Pointer to filesystem
 */
struct filesystem *
ext2_init();

#ifdef __cplusplus
}
#endif
#endif
