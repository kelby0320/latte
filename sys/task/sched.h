#ifndef SCHED_H
#define SCHED_H

struct task;

/**
 * @brief Initialize the scheduler
 * 
 * @return int  Status code
 */
int
sched_init();

/**
 * @brief Add a task to the scheduler
 * 
 * @param task  Pointer to the task
 * @return int  Status code
 */
int
sched_add_task(struct task *task);

/**
 * @brief Schedule a new task to run
 * 
 */
void
schedule();

#endif