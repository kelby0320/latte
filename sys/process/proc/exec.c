#include "proc/exec.h"

#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "kalloc.h"
#include "vm.h"
#include "proc/exit.h"
#include "process.h"
#include "thread.h"

int
process_execv(struct process *process, const char *path, const char *const *argv, size_t argv_len)
{
    int res = process_free_threads(process);
    if (res < 0) {
        goto err;
    }

    res = process_free_allocations(process);
    if (res < 0) {
        goto err;
    }

    res = process_new_vm_area(process);
    if (res < 0) {
        goto err;
    }

    res = process_load_exec(process, path);
    if (res < 0) {
        goto err;
    }

    int tid = process_add_thread(process);
    if (tid < 0) {
        goto err;
    }

    struct thread *thread = thread_get(tid);
    if (!thread) {
	goto err;
    }

    res = process_set_argv(process, thread, argv, argv_len);
    if (res < 0) {
	goto err;
    }

    return 0;

err:
    process_exit(process, PROCESS_STATUS_CODE_FAILURE);
    return res;
}
