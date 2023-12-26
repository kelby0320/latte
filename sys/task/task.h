#ifndef TASK_H
#define TASK_H

#include "irq/isr.h"

#include <stdint.h>

#define TASK_STATE_RUNNING  0
#define TASK_STATE_READY    1
#define TASK_STATE_BLOCKED  2
#define TASK_STATE_NEW      3
#define TASK_STATE_COMPLETE 4

typedef uint8_t TASK_STATE;

/**
 * @brief Saved registers
 *
 */
struct registers {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t ds;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct process;

/**
 * @brief Object that represents a task running on the system
 *
 */
struct task {
    // Task state
    TASK_STATE state;

    // Task id
    uint32_t id;

    // Saved registers
    struct registers registers;

    // The stack for this task
    void *stack;

    // Stack size
    uint32_t stack_size;

    // This tasks parent process
    struct process *process;
};

/**
 * @brief Create a new task
 *
 * @param process   Pointer to the parent process
 * @return int      Status code
 */
int
task_new(struct process *process);

/**
 * @brief Get a task
 *
 * @param tid   Task Id
 * @return struct task* Pointer to the task
 */
struct task *
task_get(uint32_t tid);

/**
 * @brief Free a task
 *
 * @param task  Pointer to the task
 */
void
task_free(struct task *task);

/**
 * @brief Switch to this task and begin executing in user space
 *
 * @param task      Pointer to the task
 */
void
task_switch_and_return(struct task *task);

/**
 * @brief Save a task's currently running context
 *
 * @param task      Pointer to the task
 * @param irq_frame Pointer to the irq_frame
 */
void
task_save_state(struct task *task, struct isr_frame *irq_frame);

#endif
