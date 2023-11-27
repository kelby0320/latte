#ifndef PROCESS_H
#define PROCESS_H

#include "config.h"
#include "mem/vm.h"

#include <stddef.h>
#include <stdint.h>

struct task;

struct process_allocation {
    void *ptr;
    size_t size;
};

struct process {
    uint32_t id;
    char filename[LATTE_MAX_PATH_LEN];
    struct vm_area *vm_area;
    struct task* tasks[LATTE_PROCESS_MAX_TASKS];
    int num_tasks;
    struct process_allocation allocations[LATTE_PROCESS_MAX_ALLOCATIONS];
};

int
process_spawn(const char *filename);

int
process_add_task(struct process *process, struct task *task);

int
process_remove_task(struct process *process, uint16_t task_id);

int
process_load_exec(struct process *process);

void
process_free(struct process *process);

#endif