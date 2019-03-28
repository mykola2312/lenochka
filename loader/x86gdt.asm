section .text
global _gdt_install
_gdt_install:
	push ebp
	mov ebp,esp
	
	; [ebp+0x08] = GDT table ptr
	; [ebp+0x0C] = GDT limit
	mov eax,dword [ebp+0x08]
	mov dword [gdt_base],eax
	mov eax,dword [ebp+0x0C]
	mov word [gdt_limit],ax
	
	lgdt [x86gdt]
	; 0x00 - NULL desc
	; 0x08 - CODE desc
	; 0x10 - DATA desc
	
	; Reloads segments
	jmp 0x08:reload_cs
reload_cs:
	mov ax,0x10
	mov ds,ax
	mov es,ax
	mov ss,ax
	mov gs,ax
	mov fs,ax
	
	mov esp,ebp
	pop ebp
	ret

section .data
x86gdt:
	gdt_limit dw 0
	gdt_base dd 0
