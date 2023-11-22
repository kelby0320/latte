    .section .text
    .global enable_paging
    .global load_page_directory

enable_paging:
    pushl   %ebp
    movl    %esp, %ebp
    movl    %cr0, %eax
    orl     $0x80000000, %eax
    movl    %eax, %cr0
    popl    %ebp
    ret

load_page_directory:
    pushl   %ebp
    movl    %esp, %ebp
    movl    8(%ebp), %eax
    movl    %eax, %cr3
    popl    %ebp
    ret