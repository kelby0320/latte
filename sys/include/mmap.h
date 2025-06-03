#ifndef PROC_MMAP_H
#define PROC_MMAP_H

#include "process.h"

#include <stddef.h>

void *
process_mmap(
    struct process *process, void *addr, size_t length, int prot, int flags,
    int fd, size_t offset);

int
process_munmap(struct process *process, void *addr, size_t length);

#endif
