#ifndef THREAD_H
#define THREAD_H

#include "irq/isr.h"

#include <stddef.h>
#include <stdint.h>

#define THREAD_STATE_RUNNING  0
#define THREAD_STATE_READY    1
#define THREAD_STATE_BLOCKED  2
#define THREAD_STATE_NEW      3
#define THREAD_STATE_COMPLETE 4

#define THREAD_STACK_SIZE 0x10000 // 64KB

#define THREAD_STACK_VIRT_ADDR_TOP    0xBFFFF000 // 3GB - 4KB
#define THREAD_STACK_VIRT_ADDR_BOTTOM (THREAD_STACK_VIRT_ADDR_TOP - THREAD_STACK_SIZE)

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
 * @brief   Thread object
 *
 */
struct thread {
    // Thread id
    uint32_t tid;

    // Thread state
    uint8_t state;

    // Saved registers
    struct registers registers;

    // The stack for this thread
    void *stack;

    // Stack size
    uint32_t stack_size;

    // This thread parent process
    struct process *process;
};

/**
 * @brief Create a new thread
 *
 * @param process   Pointer to the parent process
 * @return int      Status code
 */
int
thread_create(struct process *process);

/**
 * @brief Get a thread
 *
 * @param tid               Thread Id
 * @return struct thread*   Pointer to the thread
 */
struct thread *
thread_get(uint32_t tid);

/**
 * @brief Destroy a thread
 *
 * @param thread  Pointer to the thread
 */
void
thread_destroy(struct thread *thread);

/**
 * @brief Switch to this thread and begin executing in user space
 *
 * @param thread      Pointer to the thread
 */
void
thread_switch_and_return(struct thread *thread);

/**
 * @brief Save a thread's currently running context
 *
 * @param thread      Pointer to the thread
 * @param irq_frame Pointer to the irq_frame
 */
void
thread_save_state(struct thread *thread, struct isr_frame *irq_frame);

#endif