#include "irq/isr.h"

#include "irq/irq.h"
#include "kernel.h"
#include "mm/vm.h"
#include "port/io.h"
#include "syscall/syscall.h"
#include "task/process.h"
#include "task/sched.h"
#include "task/task.h"

void
isr_handler_wrapper(int interrupt_no, struct isr_frame *isr_frame)
{
    switch_to_kernel_vm_area();

    struct task *current_task = sched_get_current();
    task_save_state(current_task, isr_frame);

    do_irq(interrupt_no);

    vm_area_switch_map(current_task->process->vm_area);

    outb(0x20, 0x20);
}

void *
isr_syscall_handler_wrapper(int syscall_no, struct isr_frame *isr_frame)
{
    switch_to_kernel_vm_area();

    struct task *current_task = sched_get_current();
    task_save_state(current_task, isr_frame);

    void *res = do_syscall(syscall_no);

    vm_area_switch_map(current_task->process->vm_area);

    return res;
}