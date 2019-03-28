MBALIGN equ 1 << 0
MEMINFO equ 1 << 1
FLAGS equ MBALIGN|MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC+FLAGS)

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	
section .bss
global page_directories
global page_tables
global legacy_esp

align 4096
page_directories resb 4096
page_tables resb 4194304

align 16
stack_bottom:
resb 32768 ; 32 KiB
stack_top:
legacy_esp resd 1

section .text
global _start
_start:
	mov esp,(stack_bottom - 0xC0000000 + 0x7FFC)
	
	push eax
	push ebx
	
	cld
	; Our kernel will be mapped for 16M, or 4 dirs
	mov edi,(page_directories - 0xC0000000 + 0xC00)
	xor ecx,ecx
_dir_set:
	xor edx,edx
	mov eax,4096
	mul ecx
	add eax,(page_tables - 0xC0000000 + 0xC0000)
	or eax,3 ; Present + RW
	stosd
	inc ecx
	cmp ecx,4
	jne _dir_set
	
	; Setup tables - map from 0 to 16M
	mov edi,(page_tables - 0xC0000000 + 0xC0000)
	xor ebx,ebx ; Physical address
	
	mov ecx,4096
_table_set:
	mov eax,ebx
	add eax,3
	stosd
	
	add ebx,4096
	loop _table_set
	
	; Identity map also first 4 MB
	
	mov esi,(page_directories - 0xC0000000 + 0xC00)
	mov edi,(page_directories - 0xC0000000)
	mov cx,4
_copy_map:
	lodsd
	stosd
	loop _copy_map
	
	mov eax,(page_directories - 0xC0000000 + 0xC00)
	mov ebx,(page_directories - 0xC0000000)
	mov ecx,(page_tables - 0xC0000000 + 0xC0000)
	
	; Enable paging
	mov eax,(page_directories - 0xC0000000)
	mov cr3,eax
	
	mov eax,cr0
	or eax,0x80010000
	mov cr0,eax
	
	mov eax,_paging_enter
	jmp eax
	
_paging_enter:
	extern kernel_main
	
	add esp,0xC0000000
	mov dword [legacy_esp],esp
	
	;push eax
	;push ebx
	call kernel_main
	add esp,8
	cli
	hlt
.end:
