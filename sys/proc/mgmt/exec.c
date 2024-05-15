#include "proc/mgmt/exec.h"

#include "errno.h"
#include "libk/alloc.h"
#include "libk/list.h"
#include "mm/kalloc.h"
#include "mm/vm.h"
#include "proc/mgmt/exit.h"
#include "proc/process.h"
#include "thread/thread.h"

int
process_execv(struct process *process, const char *path, const char *const *argv)
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

    res = process_add_thread(process);
    if (res < 0) {
        goto err;
    }

    return 0;

err:
    process_exit(process, PROCESS_STATUS_CODE_FAILURE);
    return res;
}