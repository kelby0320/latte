    .section .text

    .global load_idt
    .global syscall_wrapper
    .global isr_wrapper_table

# void
# load_idt(struct idtr *idtr) 
load_idt:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %ebx               # *idtr
    lidt    (%ebx)
    pop     %ebp
    ret

.macro isr_wrapper num
    .global isr_wrapper_\num
isr_wrapper_\num:
    # Start of struct isr_frame
    pusha                               # Push general purpose registers
    # End of struct isr_frame

    push    %esp                        # Push pointer to isr_frame
    push    \num                        # Push the interrupt number
    call    isr_handler_wrapper
    add     $8, %esp
    popa
    iret
.endm

isr_wrapper 0
isr_wrapper 1
isr_wrapper 2
isr_wrapper 3
isr_wrapper 4
isr_wrapper 5
isr_wrapper 6
isr_wrapper 7
isr_wrapper 8
isr_wrapper 9
isr_wrapper 10
isr_wrapper 11
isr_wrapper 12
isr_wrapper 13
isr_wrapper 14
isr_wrapper 15
isr_wrapper 16
isr_wrapper 17
isr_wrapper 18
isr_wrapper 19
isr_wrapper 20
isr_wrapper 21
isr_wrapper 22
isr_wrapper 23
isr_wrapper 24
isr_wrapper 25
isr_wrapper 26
isr_wrapper 27
isr_wrapper 28
isr_wrapper 29
isr_wrapper 30

syscall_wrapper:
    # Start of struct isr_frame
    pusha
    # End of struct isr_frame

    push    %esp                        # Push pointer to isr_frame
    push    %eax                        # Push syscall number
    call    isr_syscall_wrapper
    mov     %eax, (syscall_result)      # Save syscall result
    add     $8, %esp
    popa
    mov     (syscall_result), %eax
    iret

    .section .data

isr_wrapper_table:
    .long   isr_wrapper_0
    .long   isr_wrapper_1
    .long   isr_wrapper_2
    .long   isr_wrapper_3
    .long   isr_wrapper_4
    .long   isr_wrapper_5
    .long   isr_wrapper_6
    .long   isr_wrapper_7
    .long   isr_wrapper_8
    .long   isr_wrapper_9
    .long   isr_wrapper_10
    .long   isr_wrapper_11
    .long   isr_wrapper_12
    .long   isr_wrapper_13
    .long   isr_wrapper_14
    .long   isr_wrapper_15
    .long   isr_wrapper_16
    .long   isr_wrapper_17
    .long   isr_wrapper_18
    .long   isr_wrapper_19
    .long   isr_wrapper_20
    .long   isr_wrapper_21
    .long   isr_wrapper_22
    .long   isr_wrapper_23
    .long   isr_wrapper_24
    .long   isr_wrapper_25
    .long   isr_wrapper_26
    .long   isr_wrapper_27
    .long   isr_wrapper_28
    .long   isr_wrapper_29
    .long   isr_wrapper_30

syscall_result:
    .long   0
