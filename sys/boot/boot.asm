extern kernel_main

%define MULTIBOOT2_HEADER_MAGIC 0xE85250D6
%define MULTIBOOT2_ARCH_I386 0

; Multiboot header
section .multiboot
align 8

multiboot_header_start:
dd MULTIBOOT2_HEADER_MAGIC
dd MULTIBOOT2_ARCH_I386
dd ((multiboot_header_start + 16) - multiboot_header_start)
dd -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCH_I386 + ((multiboot_header_start + 16) - multiboot_header_start))


; Reserver kernel stack
section .bss

stack_bottom:
	resb 16384							; 16 Kib
stack_top:

; Kernel entry point
section .text
global _start
_start:
	; Setup stack
	mov 	esp, stack_top

	push	ebx							; Push pointer to multiboot info structure
	push	eax							; Push magic number

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

	call 	kernel_main

; Hang if kernel returns
	cli
	jmp $
