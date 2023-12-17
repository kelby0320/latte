#ifndef PROCESS_H
#define PROCESS_H

#include "config.h"
#include "mem/vm.h"
#include "task/loader.h"

#include <stddef.h>
#include <stdint.h>

struct task;

/**
 * @brief Object that tracks memory allocations for a process
 *
 */
struct process_allocation {
    // Memory address
    void *ptr;

    // Size of allocation
    size_t size;
};

/**
 * @brief Object that represents a process on the system
 *
 */
struct process {
    // Process Id
    uint32_t id;

    // Filename of executable
    char filename[LATTE_MAX_PATH_LEN];

    // Elf image descriptor
    struct elf_img_desc elf_img_desc;

    // VM Area for the process
    struct vm_area *vm_area;

    // List of tasks associated with the process
    struct task *tasks[LATTE_PROCESS_MAX_TASKS];

    // Number of tasks associated with the process
    int num_tasks;

    // List of memory allocation for the process
    struct process_allocation allocations[LATTE_PROCESS_MAX_ALLOCATIONS];

    // Number of allocations for the process
    int num_allocations;
};

/**
 * @brief Create a new process
 *
 * @param filename  Filename
 * @return int      Status code
 */
int
process_spawn(const char *filename);

/**
 * @brief Add a new tasks to a process
 *
 * @param process   Pointer to the process
 * @param task      Pointer to the task
 * @return int      Status code
 */
int
process_add_task(struct process *process, struct task *task);

/**
 * @brief Remove a task from a process
 *
 * @param process   Pointer to the process
 * @param task_id   Task id
 * @return int      Status code
 */
int
process_remove_task(struct process *process, uint16_t task_id);

/**
 * @brief Switch to a process's vm_area
 *
 * @param process   Pointer to the process
 */
void
process_switch_to_vm_area(struct process *process);

/**
 * @brief Free a process
 *
 * @param process   Pointer to the process
 */
void
process_free(struct process *process);

#endif
