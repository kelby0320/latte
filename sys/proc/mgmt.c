#include "proc/mgmt.h"

#include "libk/alloc.h"
#include "mm/kalloc.h"
#include "proc/fd.h"
#include "proc/process.h"
#include "vfs/vfs.h"

int
process_fork(struct process *process)
{
    // TODO
    return -1;
}

int
process_wait(struct process *process)
{
    // TODO
    return -1;
}

int
process_execve(struct process *process, const char *const *argv, const char *const *envp)
{
    // TODO
    return -1;
}

int
process_exit(struct process *process, uint8_t status_code)
{
    // kill/unschedule all threads
    for_each_in_list(struct thread *, process->threads, tlist, thread) { thread_destroy(thread); }

    list_destroy(process->threads);

    // close all open file descriptors
    for_each_in_list(struct process_fd *, process->open_fds, fdlist, proc_fd)
    {
        int gfd = process_get_gfd(process, proc_fd->pfd);
        if (gfd < 0) {
            kfree(proc_fd);
            continue;
        }

        vfs_close(gfd);
        kfree(proc_fd);
    }

    list_destroy(process->open_fds);

    // free all memory allocations
    for_each_in_list(struct process_allocation *, process->allocations, alloc_list, alloc)
    {
        kalloc_free_phys_pages(alloc->addr);
        kfree(alloc);
    }

    list_destroy(process->allocations);

    // set state and status_code entry
    process->state = PROCESS_STATE_ZOMBIE;
    process->status_code = status_code;

    return 0;
}