section .text
global kmemcpy
global kmemcmp
global kmemzero

kmemcpy:
	push ebp
	mov ebp,esp
	
	; [ebp+0x08] = dst
	; [ebp+0x0C] = src
	; [ebp+0x10] = size
	
	push edi
	push esi
	
	mov edi,dword [ebp+0x08]
	mov esi,dword [ebp+0x0C]
	
	mov ecx,dword [ebp+0x10]
	shr ecx,2 ; Div by 4
	jz .cp_loop4_skip
.cp_loop4:
	lodsd
	stosd
	loop .cp_loop4
.cp_loop4_skip:
	
	mov ecx,dword [ebp+0x10]
	and ecx,3 ; Remainder by 4
	jz .cp_loop1_skip
.cp_loop1:
	lodsb
	stosb
	loop .cp_loop1
.cp_loop1_skip:
	
	pop esi
	pop edi
	
	mov esp,ebp
	pop ebp
	ret

kmemcmp:
	push ebp
	mov ebp,esp
	
	xor eax,eax
	; [ebp+0x08] = dst
	; [ebp+0x0C] = src
	; [ebp+0x10] = size
	
	push edi
	push esi
	
	mov edi,dword [ebp+0x08]
	mov esi,dword [ebp+0x0C]
	
	mov ecx,dword [ebp+0x10]
	shr ecx,2 ; Div by 4
	jz .cm_loop4_skip
.cm_loop4:
	lodsd
	cmp eax,dword [edi]
	jnz .cm_inequal
	add edi,4
	loop .cm_loop4
.cm_loop4_skip:
	
	mov ecx,dword [ebp+0x10]
	and ecx,3 ; Remainder by 4
	jz .cm_loop1_skip
.cm_loop1:
	lodsb
	cmp al,byte [edi]
	jnz .cm_inequal
	inc edi
	loop .cm_loop1
.cm_loop1_skip:
	
	xor eax,eax
	jmp .cm_equal
.cm_inequal:
	xor eax,eax
	inc eax
.cm_equal:
	pop esi
	pop edi
	
	mov esp,ebp
	pop ebp
	ret

kmemzero:
	push ebp
	mov ebp,esp
	
	; [ebp+0x08] - dst
	; [ebp+0x0C] - size
	
	push edi
	push ecx
	
	mov edi,dword [ebp+0x08]
	
	mov ecx,dword [ebp+0x0C]
	shr ecx,2 ; Dividy by 4
	jz .skip4
	
	xor eax,eax
	rep stosd
.skip4:

	mov ecx,dword [ebp+0x0C]
	and ecx,3
	rep stosd
	
	pop ecx
	pop edi
	
	mov ebp,esp
	pop ebp
	ret
