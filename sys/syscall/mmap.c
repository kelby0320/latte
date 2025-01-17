#include "syscall/mmap.h"

#include "errno.h"
#include "isr.h"
#include "mmap.h"
#include "process.h"
#include "sched.h"
#include "thread.h"
#include "userio.h"

#include <stddef.h>
#include <stdint.h>

void
do_mmap(struct thread *current_thread)
{
    size_t size = (size_t)thread_get_stack_item(current_thread, 0);
    void *vaddr = process_mmap(current_thread->process, NULL, size, 0, 0, -1, 0);
    thread_set_return_value(current_thread, (uint32_t)vaddr);
}

void
do_munmap(struct thread *current_thread)
{
    void *ptr = thread_get_stack_item(current_thread, 0);
    process_munmap(current_thread->process, ptr, 0);
    thread_set_return_value(current_thread, 0);
}
