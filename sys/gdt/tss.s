    .section .text
    .global tss_load

# void
# tss_load(int tss_segment);
tss_load:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %ax                # TSS Segment
    ltr     %ax
    pop     %ebp
    ret