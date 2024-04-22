#ifndef THREAD_USERIO_H
#define THREAD_USERIO_H

#include <stddef.h>

struct thread;

/**
 * @brief Get an item from a thread's stack
 *
 * @param thread    Pointer to the thread
 * @param index     Index of the item on the thread's stack
 * @return void*    Stack item
 */
void *
thread_get_stack_item(struct thread *thread, int index);

/**
 * @brief Copy data from a user space buffer to a kernel space buffer
 *
 * @param thread        Pointer to the thread
 * @param user_buf      Pointer to the user space buffer
 * @param kernel_buf    Pointer to the kernel space buffer
 * @param size          Number of bytes to copy
 * @return int          Number of bytes copied or negative error code
 */
int
thread_copy_from_user(struct thread *thread, void *user_buf, void *kernel_buf, size_t size);

/**
 * @brief   Copy data from a kernel space buffer to a user space buffer
 *
 * @param thread        Pointer to the thread
 * @param user_buf      Pointer to the user space buffer
 * @param kernel_buf    Pointer to the kernel space buffer
 * @param size          Number of bytes to copy
 * @return int          Number of bytes copied or negative error code
 */
int
thread_copy_to_user(struct thread *thread, void *user_buf, void *kernel_buf, size_t size);

#endif