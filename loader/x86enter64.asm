extern gdt_ptr

section .data
align 4096
gdt_limit dw 64
gdt_base dd 0

base_phys dq 0

section .text
global x86enter64

x86enter64:
	; [esp+4] - base phys addr
	mov ebx,[esp+4]
	mov dword [base_phys],ebx
	xor eax,eax
	mov dword [base_phys+4],eax
	
	;cli
	; Disable paging
	mov eax,cr0
	and eax,0x7FFFFFFF
	mov cr0,eax
	; Enable PAE
	mov eax,cr4
	or eax,(1<<5)
	mov cr4,eax
	; Setup CR3
	mov eax,dword [base_phys]
	add eax,0x1000
	mov cr3,eax
	; Enter Long Mode
	mov ecx,0xC0000080
	rdmsr
	or eax,(1<<8)
	wrmsr
	; Enable paging
	mov eax,cr0
	or eax,0x80000000
	mov cr0,eax
	; Enable Write-Protect
	mov eax,cr0
	or eax,(1<<16)
	mov cr0,eax
	; Setup GDT
	mov eax,dword [base_phys]
	mov dword [gdt_base],eax
		
	lgdt [gdt_limit]
	; Jump!
	
	jmp 0x08:_kernel_64
	    
    ;db 0xEA
    ;dd _kernel_64
    ;dw 8
use64
align 4096
_kernel_64:
	mov ax,0x10
	mov ds,ax
	mov ss,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	
	mov rax,base_phys
	mov rax,qword [rax]
	mov rdi,rax
	add rax,96
	mov rax,qword [rax] ; ELF ptr
	mov rax,qword [rax+24]
	
	jmp rax
