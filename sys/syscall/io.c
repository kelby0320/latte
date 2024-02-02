#include "syscall/io.h"

#include "errno.h"
#include "irq/isr.h"
#include "task/sched.h"
#include "task/task.h"
#include "vfs/vfs.h"

#include <stddef.h>

void *
do_open()
{
    struct task *current_task = sched_get_current();
    const char *filename = (const char *)task_stack_item(current_task, 0);
    const char *mode_str = (const char *)task_stack_item(current_task, 1);
    return (void *)vfs_open(filename, mode_str);
}

void *
do_close()
{
    struct task *current_task = sched_get_current();
    int fd = (int)task_stack_item(current_task, 0);
    return (void *)vfs_close(fd);
}

void *
do_read()
{
    struct task *current_task = sched_get_current();
    int fd = (int)task_stack_item(current_task, 0);
    char *buf = (char *)task_stack_item(current_task, 1);
    size_t count = (size_t)task_stack_item(current_task, 2);
    return (void *)vfs_read(fd, buf, count);
}

void *
do_write()
{
    struct task *current_task = sched_get_current();
    int fd = (int)task_stack_item(current_task, 0);
    const char *buf = (const char *)task_stack_item(current_task, 1);
    size_t count = (size_t)task_stack_item(current_task, 2);
    return (void *)vfs_write(fd, buf, count);
}