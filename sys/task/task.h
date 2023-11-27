#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#define TASK_STATE_RUNNING 0
#define TASK_STATE_READY 1
#define TASK_STATE_BLOCKED 2
#define TASK_STATE_NEW 3
#define TASK_STATE_COMPLETE 4

typedef uint8_t TASK_STATE;

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
    uint32_t flags;
    uint32_t esp;
    uint32_t ss; 
};

struct process;

struct task {
    TASK_STATE state;
    uint32_t id;
    struct registers registers;
    void *stack;
    uint32_t stack_size;
    struct process *process;
};

int
task_new(struct process *process);

struct task*
task_get(uint32_t tid);

void
task_free(struct task *task);

#endif