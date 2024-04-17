section .text

global thread_return

; void
; thread_return(struct registers *registers)
thread_return:
    mov     ebp, esp

    mov     ebx, [ebp+4]                ; Access registers through ebx

    push    dword [ebx+48]              ; Push the stack selector
    push    dword [ebx+44]              ; Push the stack pointer

    pushf                               ; Push flags
    pop     eax
    or      eax, 0x200                  ; Set Interrupt enable bit
    push    eax

    push    dword [ebx+32]              ; Push code selector
    push    dword [ebx+28]              ; Push IP

    mov     ax, [ebx+36]                ; Load segment registers with data selector
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    mov     eax, [ebx+24]               ; Set some general purpose registers
    mov     ecx, [ebx+20]
    mov     edx, [ebx+16]
    mov     ebp, [ebx+8]
    mov     esi, [ebx+4]
    mov     edi, [ebx]
    mov     ebx, [ebx+12]

    iret                                ; Return and execute in user land
