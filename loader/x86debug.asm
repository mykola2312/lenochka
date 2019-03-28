section .text
global x86set_wr_breakpoint

x86set_wr_breakpoint:
	mov eax,[esp+4]
	; Setup debug registers
	mov dr0,eax
	;10 01 00000000000000 10
	mov eax,11010000000000000010b
	mov dr7,eax ; 1st global breakpoint
	
	ret
