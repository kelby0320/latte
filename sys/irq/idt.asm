section .text

extern generic_isr_handler

global load_idt
global isr_table

; void
; load_idt(struct idtr *idtr); 
load_idt:
    push    ebp
    mov     ebp, esp
    mov     ebx, [ebp+8]                ; *idtr
    lidt    [ebx]
    pop     ebp
    ret

; A macro to define an ISR for a particular interrupt number
%macro make_isr 1
global int%1
int%1:
    pushad                              ; Push general purpose registers (struct interrupt_frame)
    push    esp
    push    dword %1                    ; Push interrupt number
    call    generic_isr_handler   
    add     esp, 8
    popad
    iret
%endmacro

; A macro to define an ISR table entry
%macro make_isr_tbl_entry 1
    dd int%1
%endmacro

; Create all ISRs
%assign i 0
%rep 256
make_isr i
%assign i i+1
%endrep

; A table of pointers to ISRs
isr_table:
%assign i 0
%rep 256
make_isr_tbl_entry i
%assign i i+1
%endrep