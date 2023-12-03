#include "task/task.h"

#include "config.h"
#include "errno.h"
#include "gdt/gdt.h"
#include "libk/kheap.h"
#include "libk/memory.h"
#include "mem/vm.h"
#include "task/process.h"

struct task *tslots[LATTE_TASK_MAX_TASKS];

/**
 * @brief Get a free slot for a task
 * 
 * @return int Task Id
 */
static int
get_free_slot()
{
    for (int i = 0; i < LATTE_TASK_MAX_TASKS; i++) {
        if (tslots[i] == 0) {
            return i;
        }
    }

    return -EAGAIN;
}

/**
 * @brief Assign a task to a slot
 * 
 * @param tid   Task Id
 * @param task  Pointer to task
 * @return int  Status code
 */
static int
set_slot(int tid, struct task *task)
{
    if (tslots[tid] != 0) {
        return -EAGAIN;
    }

    tslots[tid] = task;
    return 0;
}
/**
 * @brief Initialize a task
 * 
 * @param task      Pointer to the task
 * @param tid       Task Id
 * @param process   Pointer to the parent process
 * @return int      Status code
 */
static int
task_init(struct task *task, int tid, struct process *process)
{
    void *stack = kzalloc(LATTE_TASK_STACK_SIZE);
    if (!stack) {
        return -ENOMEM;
    }

    int res = vm_area_map_to(process->vm_area, (void*)LATTE_TASK_STACK_VIRT_ADDR_BOTTOM,
                    stack, stack + LATTE_TASK_STACK_SIZE, VM_PAGE_PRESENT | VM_PAGE_WRITABLE);
    if (res < 0) {
        kfree(stack);
        return res;
    }

    task->state = TASK_STATE_NEW;
    task->id = tid;
    memset(&task->registers, 0, sizeof(struct registers));
    task->stack = (void*)LATTE_TASK_STACK_VIRT_ADDR_TOP;
    task->stack_size = LATTE_TASK_STACK_SIZE;
    task->process = process;

    return 0;
}

int
task_new(struct process *process)
{
    int tid = get_free_slot();
    if (tid < 0) {
        return -EAGAIN;
    }

    struct task *task = kzalloc(sizeof(struct task));
    if (!task) {
        return -ENOMEM;
    }
    
    int res = task_init(task, tid, process);
    if (res < 0) {
        goto err_out;
    }

    res = set_slot(tid, task);
    if (res < 0) {
        goto err_out;
    }

    return tid;

err_out:
    kfree(task);
    return res;
}

struct task*
task_get(uint32_t tid)
{
    if (tid >= LATTE_TASK_MAX_TASKS) {
        return 0;
    }

    return tslots[tid];
}

void
task_free(struct task *task)
{
    // TODO
}

void
task_switch_to_vm_area(struct task *task)
{
    gdt_set_user_data_segment();
    vm_area_switch_map(task->process->vm_area);
}

void
task_save_state(struct task *task, struct irq_frame *irq_frame)
{
    task->registers.edi = irq_frame->edi;
    task->registers.esi = irq_frame->esi;
    task->registers.ebp = irq_frame->ebp;
    task->registers.ebx = irq_frame->ebx;
    task->registers.edx = irq_frame->edx;
    task->registers.ecx = irq_frame->ecx;
    task->registers.eax = irq_frame->eax;
    task->registers.ip = irq_frame->ip;
    task->registers.cs = irq_frame->cs;
    task->registers.flags = irq_frame->flags;
    task->registers.esp = irq_frame->esp;
    task->registers.ss = irq_frame->ss;
}