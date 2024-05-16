#include "sched/sched.h"

#include "errno.h"
#include "kernel.h"
#include "libk/memory.h"
#include "libk/queue.h"
#include "mm/vm.h"
#include "thread/thread.h"

static struct queue ready_queue = {0};
static struct queue blocked_queue = {0};
static struct thread *current_thread = NULL;

int
sched_add_thread(struct thread *thread)
{
    thread->state = THREAD_STATE_READY;
    return queue_enqueue(&ready_queue, thread);
}

int
sched_remove_thread(struct thread *thread)
{
    queue_remove(&ready_queue, thread);
    queue_remove(&blocked_queue, thread);

    if (current_thread == thread) {
        current_thread = NULL;
    }

    return 0;
}

int
sched_block_thread(struct thread *thread)
{
    queue_remove(&ready_queue, thread);

    if (current_thread == thread) {
        current_thread = NULL;
    }

    return queue_enqueue(&blocked_queue, thread);
}

int
sched_unblock_thread(struct thread *thread)
{
    queue_remove(&blocked_queue, thread);
    return queue_enqueue(&ready_queue, thread);
}

struct thread *
sched_get_current()
{
    return current_thread;
}

void
schedule_first_thread()
{
    current_thread = queue_dequeue(&ready_queue);
    if (!current_thread) {
        panic("No threads left to run");
    }

    current_thread->state = THREAD_STATE_RUNNING;

    thread_switch_and_return(current_thread);
}

void
schedule()
{
    if (current_thread) {
        // Enqueue currently running thread
        current_thread->state = THREAD_STATE_READY;

        int res = queue_enqueue(&ready_queue, current_thread);
        if (res < 0) {
            panic("Failed to enqueue thread");
        }
    }

    // Dequeue next thread to run
    current_thread = queue_dequeue(&ready_queue);
    if (!current_thread) {
        panic("No threads left to run");
    }

    current_thread->state = THREAD_STATE_RUNNING;

    thread_switch_and_return(current_thread);
}
