    .global latte_open
    .global latte_close
    .global latte_read
    .global latte_write
    .global latte_mmap
    .global latte_munmap

    .set OPEN_SYSCALL_NO,       0
    .set CLOSE_SYSCALL_NO,      1
    .set READ_SYSCALL_NO,       2
    .set WRITE_SYSCALL_NO,      3
    .set MMAP_SYSCALL_NO,       4
    .set MUNMAP_SYSCALL_NO,     5

    .set SYSCALL_INTERRUPT_NO,  80

    .section .text

latte_open:
    pushl   %ebp
    movl    %esp, %ebp

    movl    $OPEN_SYSCALL_NO, %eax
    pushl   8(%ebp)                     # Mode string
    pushl   12(%ebp)                    # Filename
    int     $SYSCALL_INTERRUPT_NO

    addl    $8, %esp
    popl    %ebp
    ret

latte_close:
    pushl   %ebp
    movl    %esp, %ebp

    movl    $CLOSE_SYSCALL_NO, %eax
    pushl   8(%ebp)                     # File descriptor
    int     $SYSCALL_INTERRUPT_NO

    addl    $4, %esp
    popl    %ebp
    ret

latte_read:
    pushl   %ebp
    movl    %esp, %ebp

    movl    $READ_SYSCALL_NO, %eax
    pushl   8(%ebp)                     # Count
    pushl   12(%ebp)                    # Buffer
    pushl   16(%ebp)                    # File descriptor
    int     $SYSCALL_INTERRUPT_NO

    addl    $12, %esp
    popl    %ebp
    ret

latte_write:
    pushl   %ebp
    movl    %esp, %ebp

    movl    $WRITE_SYSCALL_NO, %eax
    pushl   8(%ebp)                     # Count
    pushl   12(%ebp)                    # Buffer
    pushl   16(%ebp)                    # File descriptor
    int     $SYSCALL_INTERRUPT_NO

    addl    $12, %esp
    popl    %ebp
    ret

latte_mmap:
    pushl   %ebp
    movl    %esp, %ebp

    movl    $MMAP_SYSCALL_NO, %eax
    pushl   8(%ebp)                     # Size
    int     $SYSCALL_INTERRUPT_NO

    addl    $4, %esp
    popl    %ebp
    ret

latte_munmap:
    pushl   %ebp
    movl    %esp, %ebp

    movl    $MUNMAP_SYSCALL_NO, %eax
    pushl   8(%ebp)                     # Pointer
    int     $SYSCALL_INTERRUPT_NO

    addl    $4, %esp
    popl    %ebp
    ret
