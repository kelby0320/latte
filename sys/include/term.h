#ifndef KERNEL_TERM_H
#define KERNEL_TERM_H

#ifdef __cplusplus
extern "C" {
#endif
void
term_init();

void
term_clear_screen();

int
term_write(const char *str);

#ifdef __cplusplus
}
#endif
#endif
