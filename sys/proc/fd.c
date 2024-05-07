#include "proc/fd.h"

#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "proc/process.h"

/**
 * @brief   Find the next available process file descriptor
 *
 * @param process   Pointer to the process
 * @return int      Process file descriptor
 */
static int
find_next_pfd(struct process *process)
{
    int next_pfd = 0;
    int largest_pfd = 0;
    for_each_in_list(struct process_fd *, process->open_fds, list, fd)
    {
        if (fd->pfd > largest_pfd) {
            largest_pfd = fd->pfd;
        }

        if (fd->pfd == next_pfd) {
            next_pfd = largest_pfd + 1;
        }
    }

    return next_pfd;
}

int
process_add_gfd(struct process *process, int gfd)
{
    int pfd = find_next_pfd(process);

    struct process_fd *process_fd = kzalloc(sizeof(struct process_fd));
    if (!process_fd) {
        return -ENOMEM;
    }

    process_fd->pfd = pfd;
    process_fd->gfd = gfd;

    int res = list_push_front(&process->open_fds, process_fd);
    if (res < 0) {
        kfree(process_fd);
        return res;
    }

    return pfd;
}

int
process_get_gfd(struct process *process, int pfd)
{
    for_each_in_list(struct process_fd *, process->open_fds, list, fd)
    {
        if (fd->pfd == pfd) {
            return fd->gfd;
        }
    }

    return -ENOENT;
}

int
process_remove_pfd(struct process *process, int pfd)
{
    struct process_fd *process_fd = NULL;

    for_each_in_list(struct process_fd *, process->open_fds, list, fd)
    {
        if (fd->pfd == pfd) {
            process_fd = fd;
            break;
        }
    }

    if (process_fd == NULL) {
        return -ENOENT;
    }

    list_remove(&process->open_fds, fd);
    kfree(fd);

    return 0;
}
