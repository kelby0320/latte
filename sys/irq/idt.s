    .section .text

    .global load_idt

# void
# load_idt(struct idtr *idtr) 
load_idt:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %ebx               # *idtr
    lidt    (%ebx)
    pop     %ebp
    ret
