extern kernel_main
extern map_kernel_page_tables
extern enable_paging
extern unmap_low_mem
extern load_gdt

global _start

; Kernel entry point
section .boot.text

_start:
	; Setup boot stack segment
	mov 	esp, boot_stack_top

	push	ebx							; Push pointer to multiboot info structure
	push	eax							; Push magic number

	; Setup kernel page tables
	call	map_kernel_page_table

	; Enable paging
	call	enable_paging

	; Unmap lowmem
	call 	unmap_low_mem

	; Setup GDT
	call 	load_gdt

	; Switch to kernel stack
	mov 	esp, kernel_stack_top

	; Remap the master PIC
	mov		al, 0x11
	out 	0x20, al					; Tell master PIC

	mov		al, 0x20					; Interrupt 0x20 is where master ISR should start
	out		0x21, al

	mov		al, 0x1
	out		0x21, al
	; End remap of the master PIC

	; Reset flags
	push	0
	popf

	call	kernel_main

	; Hang if kernel returns
	cli
	jmp $

section .boot.bss

; Reserve boot stack
boot_stack_bottom:
	resb 	4096						; 4 Kib
boot_stack_top:

section .bss

; Reserve kernel stack
kernel_stack_bottom:
	resb 	16384						; 16 Kib
kernel_stack_top:
