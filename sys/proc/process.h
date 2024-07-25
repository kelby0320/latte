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

#define PROCESS_STATUS_CODE_SUCCESS 0
#define PROCESS_STATUS_CODE_FAILURE 1

#define PROCESS_ARGV_SIZE 4096
#define PROCESS_ENVP_SIZE 4096

#define ROOT_UID 0
#define ROOT_GID 0

typedef uint32_t pid_t;

/**
 * @brief Object that tracks memory allocations for a process
 *
 */
struct process_allocation {
    // Allocation type
    uint8_t type;

    // Pysical memory address
    void *paddr;

    // Virtual memory address
    void *vaddr;

    // Size of allocation
    size_t size;
};

struct process_fd {
    // Process file descriptor
    int pfd;

    // Global file descriptor
    int gfd;
};

/**
 * @brief Object that represents a process on the system
 *
 */
struct process {
    // Process Id
    pid_t pid;

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

    // Command line arguments
    char **argv;

    // Environment variables
    char **envp;

    // Elf image descriptor
    struct elf_img_desc elf_img_desc;

    // VM Area for the process
    struct vm_area *vm_area;

    // Pointer to parent process
    struct process *parent;

    // List of child processes
    struct list_item *children;

    // List of threads associated with the process
    struct list_item *threads;

    // List of memory allocations for the process
    struct list_item *allocations;

    // List of open file descriptors
    struct list_item *open_fds;
};

/**
 * @brief Get the next pid
 *
 * @return uint32_t
 */
uint32_t
process_next_pid();

/**
 * @brief   Free all threads of a process
 *
 * @param process   Pointer to the process
 * @return int      Status code
 */
int
process_free_threads(struct process *process);

/**
 * @brief   Free all memory allocations of a process
 *
 * @param process   Pointer to the process
 * @return int      Status code
 */
int
process_free_allocations(struct process *process);

/**
 * @brief   Free all file descriptors of a process
 *
 * @param process   Pointer to the process
 * @return int      Status code
 */
int
process_free_fds(struct process *process);

/**
 * @brief   Free the vm_area of a process
 *
 * @param process   Pointer to the process
 * @return int      Status code
 */
int
process_free_vm_area(struct process *process);

/**
 * @brief   Create a new vm_area for a process
 *
 * @param process   Pointer to the process
 * @return int      Status code
 */
int
process_new_vm_area(struct process *process);

/**
 * @brief Load the process's executable
 *
 * @param process   Pointer to the process
 * @param filename  Filename
 * @return int      Status code
 */
int
process_load_exec(struct process *process, const char *filename);

/**
 * @brief   Add a thread to a process
 *
 * @param process   Pointer to the process
 * @return int      Status code
 */
int
process_add_thread(struct process *process);

/**
 * @brief Add arguments to a process
 *
 * @param process    Pointer to the process
 * @param thread     Pointer to the main thread of the process
 * @param argv       Arguments vector
 * @param argv_len   Length of arguments vector
 */
int
process_set_argv(struct process *process, struct thread *thread, const char *const *argv, size_t argv_len);

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
process_remove(struct process *process);

/**
 * @brief Switch to a process's vm_area
 *
 * @param process   Pointer to the process
 */
void
process_switch_to_vm_area(struct process *process);

#endif
