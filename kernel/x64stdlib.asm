section .text
global kmemcpy
global kmemcmp
global kmemzero

kmemcpy:
	;	RDI	dst
	;	RSI	src
	;	RDX	size
	mov rcx,rdx
	shr rcx,3
	rep movsq

	mov rcx,rdx
	and rcx,7
	rep movsb
	ret
	
kmemcmp:
	mov rcx,rdx
	shr rcx,3
	jz .test1
.cmp8:
	cmpsq
	jnz .not_equal
	loop .cmp8
	
.test1:
	mov rcx,rdx
	and rcx,7
.cmp1:
	cmpsb
	jnz .not_equal
	loop .cmp1
	
	jmp .equal
.not_equal:
	xor eax,eax
	inc eax
	ret
.equal:
	xor eax,eax
	ret

kmemzero:
	xor rax,rax
	
	mov rcx,rsi
	shr rcx,3
	rep stosq
	
	mov rcx,rsi
	and rcx,7
	rep stosb
	
	ret
