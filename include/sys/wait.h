#ifndef LIBC_SYS_WAIT_H
#define LIBC_SYS_WAIT_H

#include "unistd.h"

pid_t
wait(int *status);

#endif