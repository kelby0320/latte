section .text

global load_gdt
global gdt_set_kernel_data_segment
global gdt_set_user_data_segment

; void
; load_gdt(struct gdtr *gdtr); 
load_gdt:
    push    ebp
    mov     ebp, esp
    mov     ebx, [ebp+8]                ; *gdtr
    lgdt    [ebx]
    pop     ebp
    ret

; void
; gdt_set_kernel_data_segment();
gdt_set_kernel_data_segment:
    mov     ax, 0x10                    ; Offset of kernel data segment in GDT
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    ret

; void
; gdt_set_user_data_segment();
gdt_set_user_data_segment:
    mov     ax, 0x20                    ; Offset of user data segment in GDT
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    ret