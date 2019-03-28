section .text
global _idt_install

_idt_install:
	mov rax,idt_base
	mov qword [rax],rdi
	mov rax,idt_limit
	mov word [rax],si
	
	mov rax,idt_desc
	lidt [rax]
	ret

section .data
align 16
idt_desc:
	idt_limit dw 0
	idt_base dq 0
