MBALIGN equ 1 << 0
MEMINFO equ 1 << 1
MEMVIDEO equ 1 << 2

FLAGS equ MBALIGN|MEMINFO|MEMVIDEO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC+FLAGS)

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	times 5 dd 0
	mode_type dd 0
	width dd 0
	height dd 0
	depth dd 0
	
section .bss
align 16
stack_bottom:
resb 32768 ; 32 KiB
stack_top:

section .text
global _start:function (_start.end - _start)
_start:
	mov esp,stack_bottom
	add esp,0x7FFC ; 4 byte align
	
	push eax
	push ebx
	
	extern lmain
	call lmain
	add esp,8
	
	cli
.hang:
	hlt
	jmp .hang
.end:
