#include "sched/sched.h"

#include "errno.h"
#include "kernel.h"
#include "libk/memory.h"
#include "mm/vm.h"
#include "thread/thread.h"

#define THREAD_QUEUE_LENGTH 4096

/**
 * @brief Thread queue used to schedule thread
 *
 */
struct thread_queue {
    // The queue
    struct thread *queue[THREAD_QUEUE_LENGTH];

    // Start index of queue
    int start;

    // End index of queue
    int end;

    // Length of queue
    int len;
};

static struct thread_queue ready_queue;
static struct thread_queue blocked_queue;
static struct thread *current_thread;

/**
 * @brief Enqueue a thread onto a thread queue
 *
 * @param queue     Pointer to the queue
 * @param thread    Pointer to the thread
 * @return int      Status code
 */
static int
queue_enqueue(struct thread_queue *queue, struct thread *thread)
{
    if (queue->len == THREAD_QUEUE_LENGTH) {
        return -EAGAIN;
    }

    int end = (queue->start + queue->len) % THREAD_QUEUE_LENGTH;
    queue->queue[end] = thread;
    queue->len++;
}

/**
 * @brief Dequeue a thread from a thread queue
 *
 * @param queue         Pointer to the queue
 * @param thread_out      Output pointer to the thread
 * @return int          Status code
 */
static int
queue_dequeue(struct thread_queue *queue, struct thread **thread_out)
{
    if (queue->len == 0) {
        return -EINVAL;
    }

    *thread_out = queue->queue[queue->start];
    queue->start = (queue->start + 1) % THREAD_QUEUE_LENGTH;
    queue->len--;
}

/**
 * @brief Get queue length
 *
 * @param           Pointer to the queue
 * @return int      Queue length
 */
int
queue_len(struct thread_queue *queue)
{
    return queue->len;
}

int
sched_init()
{
    memset(&ready_queue, 0, sizeof(struct thread_queue));
    memset(&blocked_queue, 0, sizeof(struct thread_queue));
    current_thread = 0;
}

int
sched_add_thread(struct thread *thread)
{
    if (thread->state != THREAD_STATE_NEW) {
        return -EINVAL;
    }

    thread->state = THREAD_STATE_READY;
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
    if (queue_len(&ready_queue) == 0) {
        panic("No threads");
    }

    queue_dequeue(&ready_queue, &current_thread);
    current_thread->state = THREAD_STATE_RUNNING;

    thread_switch_and_return(current_thread);
}

void
schedule()
{
    current_thread->state = THREAD_STATE_READY;

    int res = queue_enqueue(&ready_queue, current_thread);
    if (res < 0) {
        panic("Failed to enqueue thread");
    }

    res = queue_dequeue(&ready_queue, &current_thread);
    if (res < 0) {
        panic("Failed to dequeue thread");
    }

    current_thread->state = THREAD_STATE_RUNNING;

    thread_switch_and_return(current_thread);
}
