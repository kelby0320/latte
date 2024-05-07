section .text

global _start

extern argc
extern argv
extern envp
extern libc_init
extern program_entry

_start:

    mov     dword [argc], ecx
    mov     dword [argv], edx
    mov     dword [envp], esi

    call    libc_init
    call    program_entry
    ret
