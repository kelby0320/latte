extern kernel_main
extern map_kernel_pages
extern enable_paging
extern unmap_low_memory
extern gdt_load
extern kernel_page_directory

global _start

%define KERNEL_HIGH_HALF_START 0xC0000000
%define to_phys(addr) ((addr - KERNEL_HIGH_HALF_START))

; Kernel entry point
section .boot.text

_start:
	; Setup boot stack segment
	mov 	esp, boot_stack_top

	push	ebx							; Push pointer to multiboot info structure
	push	eax							; Push magic number

	; Setup kernel page tables
	call	map_kernel_pages

	; Load kernel page directory
	push 	to_phys(kernel_page_directory)
	call	load_page_directory
	add		esp, 4

	; Enable paging
	call	enable_paging

	; Long jump to higher half
	lea 	ecx, _higher_half_start
	jmp 	[ecx]

section .text

_higher_half_start:
	; Setup kernel stack
	mov 	esp, kernel_stack_top

	; Unmap lowmem
	call 	unmap_low_memory

	; Force TLB flush for mapping change to take effect
	mov		ecx, cr3
	mov 	cr3, ecx

	; Initialize the GDT
	call gdt_load

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
