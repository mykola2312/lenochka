use64
section .bss
align 16
resb 65536 ; 64 KiB stack
stack_top:

section .text
global _start
extern kmain

_start:
	cli
	mov rsp,stack_top
	call kmain
	
	cli
	hlt
