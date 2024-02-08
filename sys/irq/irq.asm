section .text

global enable_interrupts
global disable_interrupts

; void
; enable_interrupts()
enable_interrupts:
    sti
    ret

; void
; disable_interrupts()
disable_interrupts:
    cli
    ret