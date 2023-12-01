	.section .text
	.global load_gdt

# void
# load_gdt(struct gdtr *gdtr); 
load_gdt:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %ebx               # *gdtr
    lgdt    (%ebx)
    pop     %ebp
    ret