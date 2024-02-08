#include "syscall/io.h"

#include "config.h"
#include "errno.h"
#include "irq/isr.h"
#include "libk/kheap.h"
#include "libk/memory.h"
#include "task/sched.h"
#include "task/task.h"
#include "vfs/vfs.h"

#include <stddef.h>

void *
do_open()
{
    struct task *current_task = sched_get_current();
    const char *user_mode_str = (const char *)task_stack_item(current_task, 0);
    const char *user_filename = (const char *)task_stack_item(current_task, 1);

    char mode_str[8];
    memset(mode_str, 0, sizeof(mode_str));

    char filename[LATTE_MAX_PATH_LEN];
    memset(filename, 0, sizeof(filename));

    task_copy_from_user(current_task, (void *)user_mode_str, mode_str, sizeof(mode_str) - 1);
    task_copy_from_user(current_task, (void *)user_filename, filename, sizeof(filename) - 1);

    int res = vfs_open(filename, mode_str);
    return (void *)res;
}

void *
do_close()
{
    struct task *current_task = sched_get_current();
    int fd = (int)task_stack_item(current_task, 0);
    int res = vfs_close(fd);
    return (void *)res;
}

void *
do_read()
{
    struct task *current_task = sched_get_current();

    size_t count = (size_t)task_stack_item(current_task, 0);
    char *user_buf = (char *)task_stack_item(current_task, 1);
    int fd = (int)task_stack_item(current_task, 2);

    char *buf = kzalloc(count);
    if (!buf) {
        return (void *)-ENOMEM;
    }

    int res = vfs_read(fd, buf, count);
    if (res < 0) {
        goto out;
    }

    // task_copy_to_user(current_task, (void *)user_buf, buf, sizeof(buf) - 1);

out:
    kfree(buf);
    return (void *)res;
}

void *
do_write()
{
    struct task *current_task = sched_get_current();

    size_t count = (size_t)task_stack_item(current_task, 0);
    const char *user_buf = (const char *)task_stack_item(current_task, 1);
    int fd = (int)task_stack_item(current_task, 2);

    size_t buf_size = count + 1;
    char *buf = kzalloc(buf_size);
    if (!buf) {
        return (void *)-ENOMEM;
    }

    task_copy_from_user(current_task, (void *)user_buf, buf, count);

    int res = vfs_write(fd, buf, count);
    kfree(buf);

    return (void *)res;
}