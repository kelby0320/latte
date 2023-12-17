global _start

extern libc_start

section .text

_start:
    call libc_start
    ret