    .global _start

    .section .text

_start:
    call libc_init
    call main
    ret