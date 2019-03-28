section .text
global x64_get_cr2
global x64_rax_n_stop

x64_get_cr2:
	mov rax,cr2
	ret
x64_rax_n_stop:
	mov rax,rdi
	jmp $
