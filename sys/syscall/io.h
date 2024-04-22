#ifndef SYSCALL_IO_H
#define SYSCALL_IO_H

/**
 * @brief Execute the open system call
 *
 * @return void* Status code
 */
void *
do_open();

/**
 * @brief Execute the close system call
 *
 * @return void* Status code
 */
void *
do_close();

/**
 * @brief Execute the read system call
 *
 * @return void* Number of bytes read or negative error code
 */
void *
do_read();

/**
 * @brief Execute the write system call
 *
 * @return void* Number of bytes written or negative error code
 */
void *
do_write();

#endif