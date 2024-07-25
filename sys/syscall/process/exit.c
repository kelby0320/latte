#include "syscall/process.h"

#include "proc/mgmt/exit.h"
#include "proc/process.h"
#include "thread/userio.h"

void
do_exit(struct thread *current_thread)
{
    int status_code = (int)thread_get_stack_item(current_thread, 0);
    process_exit(current_thread->process, (uint8_t)status_code);
}
