section .text

global _int0_handler
global _int1_handler
global _int2_handler
global _int3_handler
global _int4_handler
global _int5_handler
global _int6_handler
global _int7_handler
global _int8_handler
global _int9_handler
global _int10_handler
global _int11_handler
global _int12_handler
global _int13_handler
global _int14_handler
global _int15_handler
global _int16_handler
global _int17_handler
global _int18_handler
global _int19_handler
global _int20_handler
global _int21_handler
global _int22_handler
global _int23_handler
global _int24_handler
global _int25_handler
global _int26_handler
global _int27_handler
global _int28_handler
global _int29_handler
global _int30_handler
global _int31_handler
extern idt_handler_entry

_int0_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int1_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int2_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int3_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int4_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int5_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int6_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int7_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int8_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int9_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int10_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int11_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int12_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int13_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int14_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int15_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int16_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int17_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int18_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int19_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int20_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int21_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int22_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int23_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int24_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int25_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int26_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int27_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int28_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int29_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int30_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
	
_int31_handler:
	pushad
	call near .fun1
.fun1:
	pop eax
	sub eax,6 ; call instruction size
	
	push eax
	call idt_handler_entry
	add esp,4
	
	popad
	iretd
