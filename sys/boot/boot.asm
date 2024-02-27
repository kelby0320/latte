extern kernel_main
extern early_init
extern late_init

global _start

%define KERNEL_HIGHER_HALF_START 0xC0000000
%define to_paddr(addr) ((addr - KERNEL_HIGHER_HALF_START))

; Kernel entry point
section .boot.text

_start:
	; Setup boot stack segment
	mov 	esp, boot_stack_top

	push	ebx							; Push pointer to multiboot info structure
	push	eax							; Push magic number

	call 	early_init

	; Long jump to higher half
	lea 	ecx, _higher_half_start
	jmp 	[ecx]

section .text

_higher_half_start:
	; Setup kernel stack
	mov 	esp, kernel_stack_top

	call 	late_init

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
