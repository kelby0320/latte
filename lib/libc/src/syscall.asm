section .text

global do_syscall1
global do_syscall2
global do_syscall3

%define SYSCALL_INTERRUPT_NO    0x80

; int
; do_syscall1(int syscall_no, int arg1);
do_syscall1:
    push    ebp
    mov     ebp, esp

    mov     eax, [ebp+8]               ; System call number
    push    dword [ebp+12]               ; Argument 1
    int     SYSCALL_INTERRUPT_NO

    add     esp, 4
    pop     ebp
    ret

; int
; do_syscall2(int syscall_no, int arg1, int arg2);
do_syscall2:
    push    ebp
    mov     ebp, esp

    mov     eax, [ebp+8]               ; System call number
    push    dword [ebp+12]              ; Argument 1
    push    dword [ebp+16]               ; Argument 2
    int     SYSCALL_INTERRUPT_NO

    add     esp, 8
    pop     ebp
    ret

; int
; do_syscall3(int syscall_no, int arg1, int arg2, int arg3);
do_syscall3:
    push    ebp
    mov     ebp, esp

    mov     eax, [ebp+8]               ; System call number
    push    dword [ebp+12]              ; Argument 1
    push    dword [ebp+16]              ; Argument 2
    push    dword [ebp+20]               ; Argument 3
    int     SYSCALL_INTERRUPT_NO

    add     esp, 12
    pop     ebp
    ret