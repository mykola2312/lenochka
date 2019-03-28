section .text
extern idt_call_handler

%macro push_regs 0
	pushfq
	push rax
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push rbp
	;push rsp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro pop_regs 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	;pop rsp
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	pop rax
	popfq
%endmacro

%macro int_handler 1
global int%1_handler

int%1_handler:
	push_regs
	
	mov rdi,%1
	call idt_call_handler
	
	pop_regs
	iretq

%endmacro

int_handler 0
int_handler 1
int_handler 2
int_handler 3
int_handler 4
int_handler 5
int_handler 6
int_handler 7
int_handler 8
int_handler 9
int_handler 10
int_handler 11
int_handler 12
int_handler 13
int_handler 14
int_handler 15
int_handler 16
int_handler 17
int_handler 18
int_handler 19
int_handler 20
int_handler 21
int_handler 22
int_handler 23
int_handler 24
int_handler 25
int_handler 26
int_handler 27
int_handler 28
int_handler 29
int_handler 30
int_handler 31
