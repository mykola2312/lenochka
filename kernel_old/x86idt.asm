section .text
global _idt_install
_idt_install:
	push ebp
	mov ebp,esp
	
	; [ebp+0x08] = base
	; [ebp+0x0C] = limit
	mov eax,dword [ebp+0x08]
	mov dword [idt_base],eax
	mov eax,dword [ebp+0x0C]
	mov word [idt_limit],ax
	
	lidt [idt_desc]
	
	mov esp,ebp
	pop ebp
	ret
	
section .data
idt_desc:
	idt_limit dw 0
	idt_base dd 0
