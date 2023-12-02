section .text

extern generic_interrupt_handler

global enable_interrupts
global disable_interrupts
global interrupt_table

; void
; enable_interrupts();
enable_interrupts:
    sti
    ret

; void
; disable_interrupts();
disable_interrupts:
    cli
    ret

; A macro to define an ISR for a particular interrupt number
%macro make_interrupt 1
global int%1
int%1:
    pushad                              ; Push general purpose registers (struct interrupt_frame)
    push    esp
    push    dword %1                    ; Push interrupt number
    call    generic_interrupt_handler   
    add     esp, 8
    popad
    iret
%endmacro

; A macro to define an interrupt table entry
%macro make_int_tbl_entry 1
    dd int%1
%endmacro

; Create all ISRs
%assign i 0
%rep 256
make_interrupt i
%assign i i+1
%endrep

; A table of pointers to ISRs
interrupt_table:
%assign i 0
%rep 256
make_int_tbl_entry i
%assign i i+1
%endrep
