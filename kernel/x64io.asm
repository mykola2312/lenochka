section .text
global inb
global outb
global inw
global outw
global inl
global outl
global _insw
global _outsw
global _insw256
global _outsw256

inb:
	mov rdx,rdi
	in al,dx
	ret
	
outb:
	mov rdx,rdi
	mov rax,rsi
	out dx,al
	ret
	
inw:
	mov rdx,rdi
	in ax,dx
	ret
	
outw:
	mov rdx,rdi
	mov rax,rsi
	out dx,ax
	ret
	
inl:
	mov rdx,rdi
	in eax,dx
	ret
	
outl:
	mov rdx,rdi
	mov rax,rsi
	out dx,eax
	ret

_insw:
	; 1 - RDI	port
	; 2 - RSI	buf
	; 3 - RDX	count
	mov rcx,rdx
	mov rdx,rdi
	mov rdi,rsi
	rep insw
	ret
	
_outsw:
	; 1 - RDI	port
	; 2 - RSI	buf
	; 3 - RDX	count
	mov rcx,rdx
	mov rdx,rdi
	rep outsw
	ret
	
_insw256:
	; 1 - RDI	port
	; 2 - RSI	buf
	mov rdx,rdi
	mov rdi,rsi
	mov rcx,0x100
	rep insw
	ret
	
_outsw256:
	; 1 - RDI	port
	; 2 - RSI	buf
	mov rdx,rdi
	mov rcx,0x100
	rep outsw
	ret
