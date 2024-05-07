section .text

extern isr_handler_wrapper
extern isr_syscall_handler_wrapper

global load_idt
global isr_syscall_wrapper
global isr_wrapper_table

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
%macro isr_wrapper 1
global isr_wrapper_%1
isr_wrapper_%1:
    pushad                              ; Push general purpose registers (struct interrupt_frame)
    push    esp
    push    dword %1                    ; Push interrupt number
    call    isr_handler_wrapper   
    add     esp, 8
    popad
    iret
%endmacro

; Create all ISRs
%assign i 0
%rep 256
isr_wrapper i
%assign i i+1
%endrep

; System call ISR
isr_syscall_wrapper:
    pushad                              ; Push general purpose registers (struct interrupt_frame)
    push    esp
    push    eax                         ; Push syscall number
    call    isr_syscall_handler_wrapper ; No return
    ; mov     dword[syscall_result], eax
    ; add     esp, 8
    ; popad
    ; mov     eax, [syscall_result]
    iretd

section .data

; A macro to define an ISR table entry
%macro isr_wrapper_table_entry 1
    dd isr_wrapper_%1
%endmacro

; A table of pointers to ISRs
isr_wrapper_table:
%assign i 0
%rep 256
isr_wrapper_table_entry i
%assign i i+1
%endrep

; syscall_result:
;     dd 0
