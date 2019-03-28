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
	mov edx,dword [esp+0x04]
	in al,dx
	
	ret

outb:
	mov edx,dword [esp+0x04]
	mov eax,dword [esp+0x08]
	out dx,al
	ret
	
inw:
	mov edx,dword [esp+0x04]
	in ax,dx
	ret
	
outw:
	mov edx,dword [esp+0x04]
	mov eax,dword [esp+0x08]
	out dx,ax
	ret

inl:
	mov edx,dword [esp+0x04]
	in eax,dx
	ret
	
outl:
	mov edx,dword [esp+0x04]
	mov eax,dword [esp+0x08]
	out dx,eax
	ret

_insw:
	push ebp
	mov ebp,esp
	
	; [ebp+0x08] - port
	; [ebp+0x0C] - buffer
	; [ebp+0x10] - count
	
	push edi
	push ecx
	
	mov edx,dword [ebp+0x08]
	mov edi,dword [ebp+0x0C]
	mov ecx,dword [ebp+0x10]
	
	rep insw
	
	pop ecx
	pop edi
	
	mov esp,ebp
	pop ebp
	ret
	
_outsw:
	push ebp
	mov ebp,esp
	
	; [ebp+0x08] - port
	; [ebp+0x0C] - buffer
	; [ebp+0x10] - count
	
	push esi
	push ecx
	
	mov edx,dword [ebp+0x08]
	mov esi,dword [ebp+0x0C]
	mov ecx,dword [ebp+0x10]
	
	rep outsw
	
	pop ecx
	pop esi
	
	mov esp,ebp
	pop ebp
	ret
	
_insw256:
	push edi
	push ecx
	mov edi,dword [esp+0x0C]
	mov ecx,256
	rep insw
	pop ecx
	pop edi
	ret
	
_outsw256:
	push esi
	push ecx
	mov esi,dword [esp+0x0C]
	rep outsw
	pop ecx
	pop edi
	ret
