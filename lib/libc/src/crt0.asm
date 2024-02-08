section .text

global _start

extern libc_init
extern main

_start:
    call    libc_init
    call    main
    ret
