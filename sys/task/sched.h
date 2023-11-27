#ifndef SCHED_H
#define SCHED_H

struct task;

int
sched_init();

int
sched_add_task(struct task *task);

void
schedule();

#endif