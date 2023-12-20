    # Code segment constant
    .set KERNEL_CODE_SEGMENT,   0x08
    .set KERNEL_DATA_SEGMENT,   0x10
    .set USER_CODE_SEGMENT,     (0x18 | 0x3)
    .set USER_DATA_SEGMENT,     (0x20 | 0x3)
    
    .section .text
	.global gdt_load
    .global gdt_set_kernel_data_segment
    .global gdt_set_user_data_segment
    .global gdt_fix_kernel_registers

# void
# gdt_load(struct gdtr *gdtr)
gdt_load:
    push    %ebp
    mov     %esp, %ebp
    mov     8(%ebp), %ebx               # *gdtr
    lgdt    (%ebx)
    pop     %ebp
    ret

# void
# gdt_set_kernel_data_segment()
gdt_set_kernel_data_segment:
    mov     $KERNEL_DATA_SEGMENT, %ax    # Offset of kernel data segment in GDT
    mov     %ax, %ds
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs
    ret

# void
# gdt_set_user_data_segment()
gdt_set_user_data_segment:
    mov     $USER_DATA_SEGMENT, %ax      # Offset of user data segment in GDT
    mov     %ax, %ds
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs
    ret

# void
# gdt_set_kernel_stack_segment()
gdt_fix_kernel_registers:
    mov     $KERNEL_DATA_SEGMENT, %ax    # Offset of kernel data segment in GDT
    mov     %ax, %ss
    ljmp    $KERNEL_CODE_SEGMENT, $1f
1:
    ret