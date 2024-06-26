#ifndef SCHED_H
#define SCHED_H

struct thread;

/**
 * @brief Add a thread to the scheduler
 *
 * @param thread  Pointer to the thread
 * @return int  Status code
 */
int
sched_add_thread(struct thread *thread);

/**
 * @brief   Remove a thread from the scheduler
 *
 * @param thread    Pointer to the thread
 * @return int      Status code
 */
int
sched_remove_thread(struct thread *thread);

/**
 * @brief   Move a thread to the blocked queue
 *
 * @param thread    Pointer to the thread
 * @return int      Status code
 */
int
sched_block_thread(struct thread *thread);

/**
 * @brief   Move a thread to the ready queue
 *
 * @param thread    Pointer to the thread
 * @return int      Status code
 */
int
sched_unblock_thread(struct thread *thread);

/**
 * @brief Get the currently executing thread
 *
 * @return struct thread*
 */
struct thread *
sched_get_current();

/**
 * @brief Schedule the first thread to run
 *
 */
void
schedule_first_thread();

/**
 * @brief Schedule a new thread to run
 *
 */
void
schedule();

#endif
