#ifndef __X86_UTILS_H
#define __X86_UTILS_H

#include "os.h"

struct __packed regs_s {
	u32 cr4;
	u32 cr3;
	u32 cr2;
	u32 cr0;
	
	u32 tr;
	u32 ldt;
	u32 idt;
	u32 gdt;
	
	u32 fs;
	u32 gs;
	u32 es;
	u32 ss;
	u32 ds;
	u32 cs;
	
	u32 eflags;
	
	u32 edi;
	u32 esi;
	u32 ebp;
	u32 esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	
	u32 eip;
};

void x86_get_regs(struct regs_s* regs);
void x86_print_regs();

#endif
