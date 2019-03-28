section .text
global va_start
global va_arg

va_start:
	; RDI	va_list
	mov rax,rsp
	mov rsp,rdi
	add rsp,0x30
	
	push r9
	push r8
	push rcx
	push rdx
	push rsi
	push rdi
	
	mov rsp,rax
	ret

va_arg:
	; RDI	va_list
	; RSI	index
	push rbx
	mov rbx,rsi
	shl rbx,3
	
	cmp rsi,6
	jge .va_stack
	
	mov rax,[rdi+rbx]
	
	pop rbx
	ret
.va_stack:
	sub rbx,0x30
	add rbx,0x10
	mov rax,[rbp+rbx]
	
	pop rbx
	ret
