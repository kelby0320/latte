#ifndef KERNEL_TERM_H
#define KERNEL_TERM_H

void
term_init();

void
term_clear_screen();

int
term_write(const char *str);

#endif
