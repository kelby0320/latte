section .text

global insb
global insw
global outb
global outw

; unsigned char
; insb(unsigned short port);
insb:
    push    ebp
    mov     ebp, esp

    xor     eax, eax
    mov     edx, [ebp+8]
    in      al, dx

    pop     ebp
    ret

; unsigned short
; insw(unsigned short port);
insw:
    push    ebp
    mov     ebp, esp

    xor     eax, eax
    mov     edx, [ebp+8]
    in      ax, dx

    pop     ebp
    ret

; void
; outb(unsigned short port, unsigned char val);
outb:
    push    ebp
    mov     ebp, esp

    mov     eax, [ebp+12]
    mov     edx, [ebp+8]
    out     dx, al

    pop     ebp
    ret

; void
; outw(unsigned short port, unsigned short val);
outw:
    push    ebp
    mov     ebp, esp

    mov     eax, [ebp+12]
    mov     edx, [ebp+8]
    out     dx, ax

    pop     ebp
    ret
