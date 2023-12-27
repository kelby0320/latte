#ifndef SYSCALL_H
#define SYSCALL_H

struct isr_frame;

void *
do_syscall(int syscall_no, struct isr_frame *isr_frame);

#endif