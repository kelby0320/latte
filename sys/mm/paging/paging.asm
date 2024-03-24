section .text

global paging_enable_paging
global paging_load_page_directory
global paging_flush_tlb

; void
; paging_enable_paging()
paging_enable_paging:
    push    ebp
    mov     ebp, esp
    mov     eax, cr0
    or      eax, 0x80000000
    mov     cr0, eax
    pop     ebp
    ret

; void
; paging_load_page_directory(uint32_t *page_dir)
paging_load_page_directory:
    push    ebp
    mov     ebp, esp
    mov     eax, [ebp+8]
    mov     cr3, eax
    pop     ebp
    ret

; void
; paging_flush_tlb()
paging_flush_tlb:
    push    ebp
    mov     ebp, esp
    mov     eax, cr3
    mov     cr3, eax
    pop     ebp
    ret