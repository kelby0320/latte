#include "task/sched.h"

#include "config.h"
#include "errno.h"
#include "libk/memory.h"
#include "task/task.h"

struct task_queue {
    struct task *queue[LATTE_TASK_MAX_TASKS];
    int start;
    int end;
    int len;
};

static struct task_queue ready_queue;
static struct task_queue blocked_queue;
static struct task *current_task;

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

static int
queue_dequeue(struct task_queue *queue, struct task *task_out)
{
    if (queue->len == 0) {
        return -EINVAL;
    }

    task_out = queue->queue[queue->start];
    queue->start = (queue->start + 1) % LATTE_TASK_MAX_TASKS;
    queue->len--;
}


int
sched_init()
{
    memset(&ready_queue, 0, sizeof(struct task_queue ));
    memset(&blocked_queue, 0, sizeof(struct task_queue ));
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

void
schedule()
{
    
}