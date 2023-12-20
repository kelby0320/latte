    .section .text

    .global task_return

# void
# task_return(struct registers *registers)
task_return:
    mov     %esp, %ebp

    mov     4(%ebp), %ebx               # Access registers through ebx

    push    48(%ebx)                    # Push the stack selector
    push    44(%ebx)                    # Push the stack pointer

    pushf                               # Push flags
    pop     %eax
    or      $0x200, %eax                # Set Interrupt enable bit
    push    %eax

    push    32(%ebx)                    # Push code selector
    push    28(%ebx)                    # Push IP

    mov     36(%ebx), %ax               # Load segment registers with data selector
    mov     %ax, %ds
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs

    mov     24(%ebx), %eax              # Set some general purpose registers
    mov     20(%ebx), %ecx
    mov     16(%ebx), %edx
    mov     8(%ebx), %ebp
    mov     4(%ebx), %esi
    mov     (%ebx), %edi
    mov     12(%ebx), %ebx

    iret                                # Return and execute in user land
