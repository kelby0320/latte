    .section .text

    .global enable_paging
    .global load_page_directory

# void
# enable_paging()
enable_paging:
    push   %ebp
    mov    %esp, %ebp
    mov    %cr0, %eax
    or     $0x80000000, %eax
    mov    %eax, %cr0
    pop    %ebp
    ret

# void
# load_page_directory(uint32_t *page_dir)
load_page_directory:
    push   %ebp
    mov    %esp, %ebp
    mov    8(%ebp), %eax
    mov    %eax, %cr3
    pop    %ebp
    ret