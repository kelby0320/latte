#include "syscall/process.h"

#include "libk/alloc.h"
#include "libk/memory.h"
#include "libk/string.h"
#include "proc/mgmt/exec.h"
#include "proc/mgmt/exit.h"
#include "proc/mgmt/fork.h"
#include "proc/mgmt/wait.h"
#include "proc/process.h"
#include "sched/sched.h"
#include "thread/thread.h"
#include "thread/userio.h"

#define MAX_ARGS 32
#define MAX_ARG_LEN 256

/**
 * @brief Copy userspace arguments to kernel space
 *
 * @param current_thread    The current thread
 * @param user_argv         Userspace arguments
 * @return Pointer to copied arguments
 */
static char **
copy_user_argv(struct thread *current_thread, const char *const *user_argv)
{
    // First copy the array of argument pointers from userspace
    char **argv = kzalloc(MAX_ARGS);
    thread_copy_from_user(current_thread, (void *)user_argv, argv, MAX_ARGS -1);

    // Next, copy each argument from userspace
    size_t argv_len;
    for (argv_len = 0; argv_len < MAX_ARGS; argv_len++) {
	// NULL pointer signals the end of the argument list
	if (!argv[argv_len]) {
	    break;
	}
	
	char *arg = kzalloc(MAX_ARG_LEN);
	if (!arg) {
	    goto err_alloc;
	}

	thread_copy_from_user(current_thread, (void *)argv[argv_len], arg, MAX_ARG_LEN - 1);
        argv[argv_len] = arg;
    }

err_alloc:
    // Set any remaining pointers to 0
    memset(argv + argv_len, 0, MAX_ARGS - argv_len);
    
    return argv;
}

/**
 * @brief Free arguments copied from userspace
 *
 * @param argv        The arguments array
 * @param argv_len    Length of the arguments array
 */
static void
free_argv(char **argv, size_t argv_len)
{
    for (size_t i = 0; i < argv_len; i++) {
	kfree(argv[i]);
    }

    kfree(argv);
}

void
do_execv(struct thread *current_thread)
{
    int argv_len = (int)thread_get_stack_item(current_thread, 0);
    const char *const *user_argv = (const char *const *)thread_get_stack_item(current_thread, 1);
    const char *user_path = (const char *)thread_get_stack_item(current_thread, 2);

    char path[LATTE_MAX_PATH_LEN];
    memset(path, 0, sizeof(path));

    thread_copy_from_user(current_thread, (void *)user_path, path, sizeof(path) - 1);

    char **argv = NULL;
    if (argv_len > 0) {    
	argv = copy_user_argv(current_thread, user_argv);
    }

    process_execv(current_thread->process, path, (const char * const *)argv, argv_len);

    free_argv(argv, argv_len);
}
