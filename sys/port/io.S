    .section .text
    .global insb
    .global insw
    .global outb
    .global outw

insb:
    pushl %ebp
    movl %esp, %ebp

    xorl %eax, %eax
    movl 8(%ebp), %edx
    inb %dx, %al

    popl %ebp
    ret

insw:
    pushl %ebp
    movl %esp, %ebp

    xorl %eax, %eax
    movl 8(%ebp), %edx
    inw %dx, %ax

    popl %ebp
    ret

outb:
    pushl %ebp
    movl %esp, %ebp

    movl 12(%ebp), %eax
    movl 8(%ebp), %edx
    out %al, %dx

    popl %ebp
    ret

outw:
    pushl %ebp
    movl %esp, %ebp

    movl 12(%ebp), %eax
    movl 8(%ebp), %edx
    out %ax, %dx

    popl %ebp
    ret
