#include "syscall/syscall.h"

#include "config.h"
#include "errno.h"
#include "irq/isr.h"
#include "sched/sched.h"
#include "syscall/io.h"
#include "syscall/mmap.h"
#include "syscall/process.h"
#include "thread/thread.h"

typedef void (*syscall_t)();

static syscall_t syscalls[] = {do_open,   do_close, do_read,  do_write, do_mmap,
                               do_munmap, do_fork,  do_execv, do_wait,  do_exit};

void
do_syscall(int syscall_no)
{
    struct thread *current_thread = sched_get_current();

    if (syscall_no < 0 || syscall_no >= sizeof(syscalls)) {
        thread_set_return_value(current_thread, -EINVAL);
        return;
    }

    if (!syscalls[syscall_no]) {
        thread_set_return_value(current_thread, -EINVAL);
    }

    syscalls[syscall_no](current_thread);

    return;
}