#include "sys/wait.h"

#include "syscall.h"

pid_t
wait(int *status)
{
    int res;

    do {
        res = do_syscall1(WAIT_SYSCALL_NO, (int)status);

    } while (res < 0);

    return (pid_t)res;
}