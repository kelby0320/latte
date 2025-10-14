#ifndef KERNEL_TERM_H
#define KERNEL_TERM_H

void
term_init(void);

void
term_clear_screen(void);

int
term_write(const char *str);

#endif
