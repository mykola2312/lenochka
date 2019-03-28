section .rdata
global x64_get_font

align 8
x64_font_bin:
	incbin "font.bin"

section .text

x64_get_font:
	mov rax,x64_font_bin
	ret
