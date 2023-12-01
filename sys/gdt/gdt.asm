section .text

global load_gdt

; void
; load_gdt(struct gdtr *gdtr); 
load_gdt:
    push    ebp
    mov     ebp, esp
    mov     ebx, [ebp+8]                ; *gdtr
    lgdt    [ebx]
    pop     ebp
    ret