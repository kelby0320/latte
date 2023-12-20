    .global _start

    .section .text

_start:
    call libc_start
    ret