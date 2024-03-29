section .text

global enable_paging
global load_page_directory

; void
; enable_paging()
enable_paging:
    push    ebp
    mov     ebp, esp
    mov     eax, cr0
    or      eax, 0x80000000
    mov     cr0, eax
    pop     ebp
    ret

; void
; load_page_directory(uint32_t *page_dir)
load_page_directory:
    push    ebp
    mov     ebp, esp
    mov     eax, [ebp+8]
    mov     cr3, eax
    pop     ebp
    ret
