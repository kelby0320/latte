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
 * @brief Setup processor regisers and begin executing in user space
 *
 * @param registers Pointer to registers structure
 */
void
task_return(struct registers *registers);

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

    int res = vm_area_map_to(process->vm_area, (void *)LATTE_TASK_STACK_VIRT_ADDR_BOTTOM, stack,
                             stack + LATTE_TASK_STACK_SIZE,
                             VM_PAGE_PRESENT | VM_PAGE_WRITABLE | VM_PAGE_USER);
    if (res < 0) {
        kfree(stack);
        return res;
    }

    task->state = TASK_STATE_NEW;
    task->id = tid;
    memset(&task->registers, 0, sizeof(struct registers));
    task->registers.ip = process->elf_img_desc.elf_header.e_entry;
    task->registers.cs = LATTE_USER_CODE_SEGMENT;
    task->registers.ds = LATTE_USER_DATA_SEGMENT;
    task->registers.ss = LATTE_USER_DATA_SEGMENT;
    task->registers.esp = LATTE_TASK_STACK_VIRT_ADDR_TOP;
    task->stack = (void *)LATTE_TASK_STACK_VIRT_ADDR_TOP;
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

struct task *
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
task_switch_and_return(struct task *task)
{
    process_switch_to_vm_area(task->process);
    task_return(&task->registers);
}

void
task_save_state(struct task *task, struct isr_frame *isr_frame)
{
    task->registers.edi = isr_frame->edi;
    task->registers.esi = isr_frame->esi;
    task->registers.ebp = isr_frame->ebp;
    task->registers.ebx = isr_frame->ebx;
    task->registers.edx = isr_frame->edx;
    task->registers.ecx = isr_frame->ecx;
    task->registers.eax = isr_frame->eax;
    task->registers.ip = isr_frame->ip;
    task->registers.cs = isr_frame->cs;
    task->registers.flags = isr_frame->flags;
    task->registers.esp = isr_frame->esp;
    task->registers.ss = isr_frame->ss;
}
