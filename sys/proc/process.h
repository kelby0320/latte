#ifndef PROCESS_H
#define PROCESS_H

#include "config.h"
#include "libk/list.h"
#include "proc/ld.h"
#include "thread/thread.h"

#include <stddef.h>
#include <stdint.h>

#define PROCESS_ALLOCATION_MEMORY_SEGMENT  0
#define PROCESS_ALLOCATION_PROGRAM_SEGMENT 1

#define PROCESS_STATE_CREATED  0
#define PROCESS_STATE_RUNNING  1
#define PROCESS_STATE_SLEEPING 2
#define PROCESS_STATE_STOPPED  3
#define PROCESS_STATE_ZOMBIE   4

#define ROOT_UID 0
#define ROOT_GID 0

/**
 * @brief Object that tracks memory allocations for a process
 *
 */
struct process_allocation {
    // Allocation type
    uint8_t type;

    // Memory address
    void *addr;

    // Size of allocation
    size_t size;
};

typedef struct process_allocation process_allocation_t;
typedef struct thread thread_t;

struct process;
typedef struct process *process_ptr_t;

LIST_ITEM_TYPE_PROTO(process_allocation_t);
LIST_ITEM_TYPE_PROTO(thread_t);
LIST_ITEM_TYPE_PROTO(process_ptr_t);

/**
 * @brief Object that represents a process on the system
 *
 */
struct process {
    // Process Id
    uint32_t pid;

    // User Id
    uint16_t uid;

    // Group Id
    uint16_t gid;

    // Process state
    uint8_t state;

    // Process exit status code
    uint8_t status_code;

    // Filename of executable
    char filename[LATTE_MAX_PATH_LEN];

    // Elf image descriptor
    struct elf_img_desc elf_img_desc;

    // VM Area for the process
    struct vm_area *vm_area;

    // Pointer to parent process
    struct process *parent;

    // List of child processes
    LIST(process_ptr_t) children;

    // List of threads associated with the process
    LIST(thread_t) threads;

    // List of memory allocations for the process
    LIST(process_allocation_t) allocations;
};

/**
 * @brief   Create the first user process on the system
 *
 * @param filename  Filename of the executable
 * @return int      Status code
 */
int
process_create_first(const char *filename);

/**
 * @brief   Remove a process from the system
 *
 * @param process   Pointer to the process
 * @return int      Status code
 */
int
process_destroy(struct process *process);

/**
 * @brief Switch to a process's vm_area
 *
 * @param process   Pointer to the process
 */
void
process_switch_to_vm_area(struct process *process);

/**
 * @brief   Add a thread to the process
 *
 * @param process   Pointer to the process
 * @param thread    Pointer to the thread
 * @return int      Status code
 */
int
process_add_thread(struct process *process, struct thread *thread);

#endif