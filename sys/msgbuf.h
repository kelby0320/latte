#ifndef MSGBUF_H
#define MSGBUF_H

/**
 * @brief Write a message to the kernel message buffer
 *
 * @param msg   The message
 * @return int  Status code
 */
int
msgbuf_write(const char *msg);

/**
 * @brief Add an output file descriptor for the message buffer
 *
 * @param fd    The file descriptor
 * @return int  Status code
 */
int
msgbuf_add_output_fd(int fd);

int
msgbuf_flush();

#endif