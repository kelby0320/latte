	# Constants
	.set MULTIBOOT2_HEADER_MAGIC, 	0xE85250D6			# Multiboot2 magic number
	.set MULTIBOOT2_ARCH_I386,		0					# 32-bit protected mode


	# Multiboot header
	.section .multiboot
	.align 	8
multiboot_header_start:
	.long 	MULTIBOOT2_HEADER_MAGIC
	.long 	MULTIBOOT2_ARCH_I386
	.long 	multiboot_header_end - multiboot_header_start
	.long	-(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCH_I386 + (multiboot_header_end - multiboot_header_start))
multiboot_header_end:

	# Reserver kernel stack
	.section .bss
stack_bottom:
	.skip 16384							# 16 Kib
stack_top:

	# Kernel entry point
	.section .text
	.global _start
_start:
	# Setup stack
	mov		$stack_top, %esp

	push 	%ebx						# Push pointer to multiboot info structure
	push	%eax						# Push magic number

	# Remap the master PIC
	movb 	$0x11, %al
	outb	%al, $0x20					# Tell master PIC

	movb	$0x20, %al
	outb	%al, $0x21					# Interrupt 0x20 is where master ISR should start

	movb	$0x1, %al
	outb	%al, $0x21
	# End remap
	
	# Reset flags
	push 	$0
	popf	

	call 	kernel_main

	# Hang if kernel returns
	cli
1:	hlt
	jmp 1b
	
