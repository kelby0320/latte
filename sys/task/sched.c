#include "task/sched.h"

#include "config.h"
#include "errno.h"
#include "kernel.h"
#include "libk/memory.h"
#include "mem/vm.h"
#include "task/task.h"

/**
 * @brief Task queue used to schedule tasks
 *
 */
struct task_queue {
    // The queue
    struct task *queue[LATTE_TASK_MAX_TASKS];

    // Start index of queue
    int start;

    // End index of queue
    int end;

    // Length of queue
    int len;
};

static struct task_queue ready_queue;
static struct task_queue blocked_queue;
static struct task *current_task;

/**
 * @brief Enqueue a task onto a task queue
 *
 * @param queue     Pointer to the queue
 * @param task      Pointer to the task
 * @return int      Status code
 */
static int
queue_enqueue(struct task_queue *queue, struct task *task)
{
    if (queue->len == LATTE_TASK_MAX_TASKS) {
        return -EAGAIN;
    }

    int end = (queue->start + queue->len) % LATTE_TASK_MAX_TASKS;
    queue->queue[end] = task;
    queue->len++;
}

/**
 * @brief Dequeue a task from a task queue
 *
 * @param queue         Pointer to the queue
 * @param task_out      Output pointer to the task
 * @return int          Status code
 */
static int
queue_dequeue(struct task_queue *queue, struct task **task_out)
{
    if (queue->len == 0) {
        return -EINVAL;
    }

    *task_out = queue->queue[queue->start];
    queue->start = (queue->start + 1) % LATTE_TASK_MAX_TASKS;
    queue->len--;
}

/**
 * @brief Get queue length
 *
 * @param           Pointer to the queue
 * @return int      Queue length
 */
int
queue_len(struct task_queue *queue)
{
    return queue->len;
}

int
sched_init()
{
    memset(&ready_queue, 0, sizeof(struct task_queue));
    memset(&blocked_queue, 0, sizeof(struct task_queue));
    current_task = 0;
}

int
sched_add_task(struct task *task)
{
    if (task->state != TASK_STATE_NEW) {
        return -EINVAL;
    }

    task->state = TASK_STATE_READY;
    return queue_enqueue(&ready_queue, task);
}

struct task *
sched_get_current()
{
    return current_task;
}

void
schedule_first_task()
{
    if (queue_len(&ready_queue) == 0) {
        panic("No tasks");
    }

    queue_dequeue(&ready_queue, &current_task);
    current_task->state = TASK_STATE_RUNNING;

    task_switch_and_return(current_task);
}

void
schedule()
{
    current_task->state = TASK_STATE_READY;

    int res = queue_enqueue(&ready_queue, current_task);
    if (res < 0) {
        panic("Failed to enqueue task");
    }

    res = queue_dequeue(&ready_queue, &current_task);
    if (res < 0) {
        panic("Failed to dequeue task");
    }

    current_task->state = TASK_STATE_RUNNING;

    task_switch_and_return(current_task);
}
