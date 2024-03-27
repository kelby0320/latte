#ifndef LIBK_ALLOC_H
#define LIBK_ALLOC_H

#include <stddef.h>

/**
 * @brief   Initialize the libk allocators
 *
 */
void
libk_alloc_init();

/**
 * @brief   Allocate a block of memory
 *
 * vmalloc allocates memory blocks greater than or equal to 4096 bytes
 * and garuntees that the return address points to virtually
 * continuous memory
 *
 * @param size      The size of the block
 * @return void*    Virtual address of the block
 */
void *
vmalloc(size_t size);

/**
 * @brief   Allocate a block of memory and zero it
 *
 * vzalloc allocates memory blocks greater than or equal to 4096 bytes
 * and garuntees that the return address points to virtually
 * continuous memory
 * @param size      The size of the block
 * @return void*    Virtual address of the block
 */
void *
vzalloc(size_t size);

/**
 * @brief   Free a block of memory allocted with a vmalloc family function
 *
 */
void
vfree();

/**
 * @brief   Allocate a block of memory
 *
 * @param size      The size of the block
 * @return void*    Virtual address of the block
 */
void *
kmalloc(size_t size);

/**
 * @brief   Allocate a block of memory and zero it
 *
 * @param size      The size of the block
 * @return void*    Virtual address of the block
 */
void *
kzalloc(size_t size);

/**
 * @brief   Free a block of memory allocated with a kmalloc family function
 *
 * @param ptr    The virtual address of the block
 */
void
kfree(void *ptr);

#endif