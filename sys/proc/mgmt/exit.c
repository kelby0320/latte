#include "proc/mgmt/exit.h"

#include "libk/alloc.h"
#include "libk/list.h"
#include "mm/kalloc.h"
#include "proc/fd.h"
#include "proc/process.h"
#include "sched/sched.h"
#include "thread/thread.h"
#include "vfs/vfs.h"

int
process_exit(struct process *process, uint8_t status_code)
{
    process_free_threads(process);

    process_free_fds(process);

    process_free_allocations(process);

    process_free_vm_area(process);

    // set state and status_code entry
    process->state = PROCESS_STATE_ZOMBIE;
    process->status_code = status_code;

    if (process->parent) {
        for_each_in_list(struct thread *, process->parent->threads, list, thread)
        {
            sched_unblock_thread(thread);
        }
    }

    return 0;
}
