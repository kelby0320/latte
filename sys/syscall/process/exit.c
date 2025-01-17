#include "syscall/process.h"

#include "proc/exit.h"
#include "process.h"
#include "userio.h"

void
do_exit(struct thread *current_thread)
{
    int status_code = (int)thread_get_stack_item(current_thread, 0);
    process_exit(current_thread->process, (uint8_t)status_code);
}
