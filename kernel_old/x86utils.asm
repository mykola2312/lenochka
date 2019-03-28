section .text
global x86_get_regs
global x86_reload_cr3
global _load_legacy_stack
extern legacy_esp

x86_get_regs:
	push ebp
	mov ebp,esp
	sub esp,8
	
	; [ebp+0x08] - struct regs
	; [ebp-0x04] - saved edi
	; [ebp-0x08] - saved ecx

	;mov edx,0xCAFEBABE
	
	mov dword [ebp-0x04],edi
	mov dword [ebp-0x08],ecx
	;mov esi,esp
	;lea esi,[esp-76]
	mov edi,dword [ebp+0x08]
	
	pushad ; Push all general-purpose registers
	pushfd ; Push flags

	; Get selectors
	push cs
	push ds
	push ss
	push es
	push gs
	push fs

	; Store GDT
	sub esp,4
	sgdt [esp]
	; Store IDT
	sub esp,4
	sidt [esp]
	; Store LDT
	sub esp,4
	sldt [esp]
	; Store TR
	sub esp,4
	str [esp]
	; Store machine status word
	;sub esp,4
	;smsw [esp]
	
	mov eax,cr0
	push eax
	mov eax,cr2
	push eax
	mov eax,cr3
	push eax
	mov eax,cr4
	push eax
	
	mov ecx,23 ; 8 regs + eflags + 6 segs + control regs
.pop_loop:
	pop eax
	stosd
	loop .pop_loop
	
	; Restore everything
	mov edi,dword [ebp-0x04]
	mov ecx,dword [ebp-0x08]
	
	mov eax,dword [ebp+0x08]
	mov dword [eax+44],edi
	mov dword [eax+68],ecx
	
	mov esp,ebp
	pop ebp
	
	; Restore EBP,ESP,EIP
	mov dword [eax+52],ebp
	mov dword [eax+56],esp
	add dword [eax+56],8 ; Skip ret + 1st arg
	
	; pop EIP and push it again to to do ret
	pop dword [eax+76]
	push dword [eax+76]
	
	ret

x86_reload_cr3:
	mov eax,cr3
	mov cr3,eax
	ret
	
_load_legacy_stack:
	pop eax
	mov esp,dword [legacy_esp]
	jmp eax
