section .text

extern generic_interrupt_handler

global enable_interrupts
global disable_interrupts

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

%macro interrupt 1
global interrupt%1
int%1:
    pushad                              ; Push general purpose registers (struct interrupt_frame)
    push    esp
    push    dword %1                    ; Push interrupt number
    call    generic_interrupt_handler   
    add     esp, 8
    popad
    iret
%endmacro

%assign i 0
%rep 256
interrupt i
%assign i i+1
%endrep
