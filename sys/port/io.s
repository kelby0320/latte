    .section .text

    .global insb
    .global insw
    .global outb
    .global outw

# unsigned char
# insb(unsigned short port)
insb:
    push    %ebp
    mov     %esp, %ebp

    xor     %eax, %eax
    mov     8(%ebp), %edx               # port
    in      %dx, %al

    pop     %ebp
    ret

# unsigned short
# insw(unsigned short port)
insw:
    push    %ebp
    mov     %esp, %ebp

    xor     %eax, %eax
    mov     8(%ebp), %edx               # port
    in      %dx, %ax

    pop     %ebp
    ret

# void
# outb(unsigned short port, unsigned char val)
outb:
    push    %ebp
    mov     %esp, %ebp

    mov     12(%ebp), %eax              # val
    mov     8(%ebp), %edx               # port
    out     %al, %dx

    pop     %ebp
    ret

# void
# outw(unsigned short port, unsigned short val)
outw:
    push    %ebp
    mov     %esp, %ebp

    mov     12(%ebp), %eax              # val
    mov     8(%ebp), %edx               # port
    out     %ax, %dx

    pop     %ebp
    ret
