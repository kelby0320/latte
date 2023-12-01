extern kernel_main

; Constants
%define ALIGN (1<<0)					; Align loaded modules on page boundaries
%define MEMINFO (1<<1)					; Provide memory map
%define FLAGS (ALIGN | MEMINFO)			; Multiboot 'flag' field
%define MAGIC 0x1BADB002				; 'Magic Number'
%define CHECKSUM (-(MAGIC + FLAGS))		; Checksum to prove we are multiboot

; Multiboot header
section .multiboot
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM

; Reserver kernel stack
section .bss

stack_bottom:
	resb 16384							; 16 Kib
stack_top:

; Kernel entry point
section .text
global _start
_start:
	mov 	esp, stack_top
	call 	kernel_main

; Hang if kernel returns
	cli
	jmp $
	
