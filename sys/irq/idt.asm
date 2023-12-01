section .text

global load_idt

; void
; load_idt(struct idtr *idtr); 
load_idt:
    push    ebp
    mov     ebp, esp
    mov     ebx, [ebp+8]                ; *idtr
    lidt    [ebx]
    pop     ebp
    ret