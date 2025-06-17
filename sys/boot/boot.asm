extern kernel_main
extern early_init
extern late_init

global _start
global boot_enable_paging
global boot_load_page_directory

; Kernel entry point
section .boot.text

_start:
	;; Setup boot stack segment
	mov	esp, boot_stack_top

	push	ebx	    	; Save pointer to multiboot info structure
	push	eax	    	; Save magic number

	call	early_init

	;; Long jump to higher half
	pop	eax		; Restore magic number
	pop	ebx		; Restore multiboot info pointer
	lea	ecx, _higher_half_start
	jmp	ecx
	
;; void
;; boot_enable_paging()
boot_enable_paging:
	push	ebp
	mov	ebp, esp
	mov	eax, cr0
	or	eax, 0x80000000
	mov	cr0, eax
	pop	ebp
	ret

;; void
;; boot_load_page_directory(uint32_t *page_dir)
boot_load_page_directory:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp+8]
	mov	cr3, eax
	pop	ebp
	ret

section .text

_higher_half_start:
	;; Setup kernel stack
	mov	esp, kernel_stack_top
	
	push	ebx	    	; Save pointer to multiboot info structure
	push	eax	    	; Save magic number
	
	call	late_init

	;; Reset flags
	push	0
	popf

	call	kernel_main

	;; Hang if kernel returns
	cli
	jmp	$

section .boot.bss

;; Reserve boot stack
boot_stack_bottom:
	resb	4096		; 4 Kib
boot_stack_top:

section .bss

;; Reserve kernel stack
kernel_stack_bottom:
	resb	65536		; 64 Kib
kernel_stack_top:
