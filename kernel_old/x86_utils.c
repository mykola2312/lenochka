#include "x86utils.h"
#include "vga_terminal.h"
#include "termlib.h"

void x86_print_regs()
{
	struct regs_s regs;
	x86_get_regs(&regs);
	
	chg_color(VGA_COLOR_WHITE);
	kprintf(
		"[========\t\tx86 register dump\t\t========]\n"
		"\tEAX\t%x\tECX\t%x\n\tEDX\t%x\tEBX\t%x\n"
		"\tESP\t%x\tEBP\t%x\n\tESI\t%x\tEDI\t%x\n"
		"\n\tEIP\t%x\n"
		"\tEFLAGS\t%x\n"
		"\n\tCS %x DS %x SS %x\n"
		"\tES %x FS %x GS %x\n"
		"\n\tGDT\t%x\n\tIDT\t%x\n\tLDT\t%x\n\tTR\t%x\n"
		"\n\tCR0\t%x\tCR2\t%x\n"
		"\tCR3\t%x\tCR4\t%x\n"
		"[========\t\tx86 register dump\t\t========]\n",
		regs.eax,regs.ecx,regs.edx,regs.ebx,
		regs.esp,regs.ebp,regs.esi,regs.edi,
		regs.eip,
		regs.eflags,
		regs.cs,regs.ds,regs.ss,
		regs.es,regs.fs,regs.gs,
		regs.gdt,
		regs.idt,
		regs.ldt,
		regs.tr,
		regs.cr0,regs.cr2,
		regs.cr3,regs.cr4
	);
	chg_color(VGA_COLOR_MAGENTA);
}
