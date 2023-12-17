section .text

global gdt_load
global gdt_set_kernel_data_segment
global gdt_set_user_data_segment
global gdt_fix_kernel_registers

%define KERNEL_CODE_SEGMENT 0x08
%define KERNEL_DATA_SEGMENT 0x10
%define USER_CODE_SEGMENT (0x18 | 0x3)
%define USER_DATA_SEGMENT (0x20 | 0x3)

; void
; gdt_load(struct gdtr *gdtr); 
gdt_load:
    push    ebp
    mov     ebp, esp
    mov     ebx, [ebp+8]                ; *gdtr
    lgdt    [ebx]
    pop     ebp
    ret

; void
; gdt_set_kernel_data_segment();
gdt_set_kernel_data_segment:
    mov     ax, KERNEL_DATA_SEGMENT     ; Offset of kernel data segment in GDT
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    ret

; void
; gdt_set_user_data_segment();
gdt_set_user_data_segment:
    mov     ax, USER_DATA_SEGMENT       ; Offset of user data segment in GDT
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    ret

; void
; gdt_set_kernel_stack_segment()
gdt_fix_kernel_registers:
    mov ax, KERNEL_DATA_SEGMENT         ; Offset of kernel data segment in GDT
    mov ss, ax
    jmp KERNEL_CODE_SEGMENT:.test
.test
    ret