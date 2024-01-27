#include "msgbuf.h"

#include "config.h"
#include "errno.h"
#include "libk/string.h"

static char message_buffer[MSGBUF_BUFFER_SIZE] = {0};
static unsigned int msgbuf_len = 0;

static int output_fds[MSGBUF_MAX_OUTPUT_FDS] = {0};
static unsigned int outfds_len = 0;

static const char buffer_overflow_msg[] = "Kernel Message Buffer Overflow!";

/**
 * @brief Message buffer overflow handler
 *
 * On buffer overflow write the buffer_overflow_msg to the message buffer
 *
 */
static void
msgbuf_buffer_overflow()
{
    size_t msglen = strlen(buffer_overflow_msg);
    char *dest = message_buffer + (MSGBUF_BUFFER_SIZE - msglen);
    strncpy(dest, buffer_overflow_msg, msglen);
}

int
msgbuf_write(const char *msg)
{
    size_t msglen = strlen(msg);
    if (msglen > (MSGBUF_BUFFER_SIZE - msgbuf_len)) {
        msgbuf_buffer_overflow();
        return -EIO;
    }

    char *dest = message_buffer + msgbuf_len;
    strncpy(dest, msg, msglen);

    msgbuf_len += msglen;

    return 0;
}

int
msgbuf_add_output_fd(int fd)
{
    if (outfds_len == MSGBUF_MAX_OUTPUT_FDS) {
        return -EAGAIN;
    }

    output_fds[outfds_len] = fd;
    outfds_len++;
}