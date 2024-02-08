section .text
    
global latte_open
global latte_close
global latte_read
global latte_write
global latte_mmap
global latte_munmap

%define OPEN_SYSCALL_NO         0
%define CLOSE_SYSCALL_NO        1
%define READ_SYSCALL_NO         2
%define WRITE_SYSCALL_NO        3
%define MMAP_SYSCALL_NO         4
%define MUNMAP_SYSCALL_NO       5

%define SYSCALL_INTERRUPT_NO    0x80

; int
; latte_open(const char *filename, const char *mode_str)
latte_open:
    push    ebp
    mov     ebp, esp

    mov     eax, OPEN_SYSCALL_NO
    push    dword [ebp+8]                     ; Mode string
    push    dword [ebp+12]                    ; Filename
    int     SYSCALL_INTERRUPT_NO

    add     esp, 8
    pop     ebp
    ret

; int
; latte_close(int fd)
latte_close:
    push    ebp
    mov     ebp, esp

    mov     eax, CLOSE_SYSCALL_NO
    push    dword [ebp+8]                     ; File descriptor
    int     SYSCALL_INTERRUPT_NO

    add     esp, 4
    pop     ebp
    ret

; int
; latte_read(int fd, char *ptr, size_t count)
latte_read:
    push    ebp
    mov     ebp, esp

    mov     eax, READ_SYSCALL_NO
    push    dword [ebp+8]               ; Count
    push    dword [ebp+12]              ; Buffer
    push    dword [ebp+16]              ; File descriptor
    int      SYSCALL_INTERRUPT_NO

    add     esp, 12
    pop     ebp
    ret

; int
; latte_write(int fd, const char *ptr, size_t count)
latte_write:
    push    ebp
    mov     ebp, esp

    mov     eax, WRITE_SYSCALL_NO
    push    dword [ebp+8]               ; Count
    push    dword [ebp+12]              ; Buffer
    push    dword [ebp+16]              ; File descriptor
    int      SYSCALL_INTERRUPT_NO

    add     esp, 12
    pop     ebp
    ret

; void *
; latte_mmap(size_t size)
latte_mmap:
    push    ebp
    mov     ebp, esp

    mov     eax, MMAP_SYSCALL_NO
    push    dword [ebp+8]               ; Size
    int     SYSCALL_INTERRUPT_NO

    add     esp, 4
    pop     ebp
    ret

; int
; latte_munmap(void *ptr)
latte_munmap:
    push    ebp
    mov     ebp, esp

    mov     eax, MUNMAP_SYSCALL_NO
    push    dword [ebp+8]               ; Pointer
    int     SYSCALL_INTERRUPT_NO

    add     esp, 4
    pop     ebp
    ret
