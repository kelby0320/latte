#include "syscall/io.h"

#include "config.h"
#include "errno.h"
#include "irq/isr.h"
#include "libk/alloc.h"
#include "libk/memory.h"
#include "proc/fd.h"
#include "sched/sched.h"
#include "thread/thread.h"
#include "thread/userio.h"
#include "vfs/file_descriptor.h"
#include "vfs/vfs.h"

#include <stddef.h>

void
do_open(struct thread *current_thread)
{
    const char *user_mode_str = (const char *)thread_get_stack_item(current_thread, 0);
    const char *user_filename = (const char *)thread_get_stack_item(current_thread, 1);

    char mode_str[8];
    memset(mode_str, 0, sizeof(mode_str));

    char filename[LATTE_MAX_PATH_LEN];
    memset(filename, 0, sizeof(filename));

    thread_copy_from_user(current_thread, (void *)user_mode_str, mode_str, sizeof(mode_str) - 1);
    thread_copy_from_user(current_thread, (void *)user_filename, filename, sizeof(filename) - 1);

    int gfd = vfs_open(filename, mode_str);
    if (gfd < 0) {
        thread_set_return_value(current_thread, gfd);
        return;
    }

    int pfd = process_add_gfd(current_thread->process, gfd);
    thread_set_return_value(current_thread, pfd);
}

void
do_close(struct thread *current_thread)
{
    int pfd = (int)thread_get_stack_item(current_thread, 0);

    int gfd = process_get_gfd(current_thread->process, pfd);
    if (gfd < 0) {
        thread_set_return_value(current_thread, -ENOENT);
        return;
    }

    int res = vfs_close(gfd);
    thread_set_return_value(current_thread, res);
}

void
do_read(struct thread *current_thread)
{
    size_t count = (size_t)thread_get_stack_item(current_thread, 0);
    char *user_buf = (char *)thread_get_stack_item(current_thread, 1);
    int pfd = (int)thread_get_stack_item(current_thread, 2);

    int gfd = process_get_gfd(current_thread->process, pfd);
    if (gfd < 0) {
        thread_set_return_value(current_thread, -ENOENT);
        return;
    }

    char *buf = kzalloc(count);
    if (!buf) {
        thread_set_return_value(current_thread, -ENOMEM);
        return;
    }

    int res = vfs_read(gfd, buf, count);
    if (res < 0) {
        goto out_free_buf;
    }

    res = thread_copy_to_user(current_thread, (void *)user_buf, buf, count);

out_free_buf:
    kfree(buf);
    thread_set_return_value(current_thread, res);
}

void
do_write(struct thread *current_thread)
{
    size_t count = (size_t)thread_get_stack_item(current_thread, 0);
    const char *user_buf = (const char *)thread_get_stack_item(current_thread, 1);
    int pfd = (int)thread_get_stack_item(current_thread, 2);

    int gfd = process_get_gfd(current_thread->process, pfd);
    if (gfd < 0) {
        thread_set_return_value(current_thread, -ENOENT);
        return;
    }

    size_t buf_size = count + 1;
    char *buf = kzalloc(buf_size);
    if (!buf) {
        thread_set_return_value(current_thread, -ENOMEM);
        return;
    }

    int res = thread_copy_from_user(current_thread, (void *)user_buf, buf, count);
    if (res < 0) {
        goto out_free_buf;
    }

    res = vfs_write(gfd, buf, count);

out_free_buf:
    kfree(buf);
    thread_set_return_value(current_thread, res);
}

void
do_opendir(struct thread *current_thread)
{
    const char *user_dirname = (const char *)thread_get_stack_item(current_thread, 0);
    char dirname[LATTE_MAX_PATH_LEN];
    memset(dirname, 0, sizeof(dirname));

    thread_copy_from_user(current_thread, (void *)user_dirname, dirname, sizeof(dirname) - 1);

    int gfd = vfs_opendir(dirname);
    if (gfd < 0) {
	thread_set_return_value(current_thread, gfd);
	return;
    }

    int pfd = process_add_gfd(current_thread->process, gfd);
    thread_set_return_value(current_thread, pfd);
}

void
do_closedir(struct thread *current_thread)
{
    int pfd = (int)thread_get_stack_item(current_thread, 0);

    int gfd = process_get_gfd(current_thread->process, pfd);
    if (gfd < 0) {
        thread_set_return_value(current_thread, -ENOENT);
        return;
    }

    int res = vfs_closedir(gfd);
    thread_set_return_value(current_thread, res);
}

void
do_readdir(struct thread *current_thread)
{
    char *user_buf = (char *)thread_get_stack_item(current_thread, 0);
    int pfd = (int)thread_get_stack_item(current_thread, 1);

    int gfd = process_get_gfd(current_thread->process, pfd);
    if (gfd < 0) {
        thread_set_return_value(current_thread, -ENOENT);
        return;
    }

    size_t buf_size = sizeof(struct dir_entry);
    char *buf = kzalloc(buf_size);
    if (!buf) {
        thread_set_return_value(current_thread, -ENOMEM);
        return;
    }

    int res = vfs_readdir(gfd, (struct dir_entry *)buf);
    if (res < 0) {
        goto out_free_buf;
    }

    res = thread_copy_to_user(current_thread, (void *)user_buf, buf, buf_size);

out_free_buf:
    kfree(buf);
    thread_set_return_value(current_thread, res);    
}
