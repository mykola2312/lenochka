#include "exceptions.h"
#include "vga_terminal.h"
#include "stdlib.h"
#include "idt.h"
#include "pit.h"
//#include "x86utils.h"

/* We need to implement those excpetions:
 * 0 Division by zero
 * 3 Breakpoint
 * 6 Invalid opcode
 * 7 Device not available
 * 8 Double Fault
 * 10 Invalid TSS
 * 11 Segment not present
 * 12 Stack segment fault
 * 13 General protection fault
 * 14 Page fault
 * 
 */

static void hang()
{
	//x86_print_regs();
	while(1){__asm("nop");}
}

//extern void command_interpreter();
//extern void _load_legacy_stack();

void _enter_rescue_mode()
{
	/*_load_legacy_stack();
	sleep(500);
	command_interpreter();*/
}

void enter_rescue_mode(int skip_check)
{
	if(!skip_check)
	{
		chg_color(VGA_COLOR_LIGHT_CYAN);
		kprintf("Rollback to terminal? (y - yes, n - no): ");
		u8 ans = kgetc();
		if(ans == 'y' || skip_check)
		{
			kputc('\n');
			chg_color(VGA_COLOR_MAGENTA);
			_enter_rescue_mode();
		}
		chg_color(VGA_COLOR_MAGENTA);
	}
	else
	{
		cls();
		kprintf("Terminal reloaded\n");
		_enter_rescue_mode();
	}
}

void exception0()
{
	kprintf("[Exception] Division by zero\n");
	hang();
}

void exception3()
{
	kprintf("[Exception] Debug\n");
	hang();
}

void exception6()
{
	kprintf("[Exception] Invalid opcode\n");
	hang();
}

void exception7()
{
	kprintf("[Exception] Device not available\n");
	hang();
}

void exception8()
{
	kprintf("[Exception] Double fault\n");
	hang();
}

void exception10()
{
	kprintf("[Exception] Invalid TSS\n");
	hang();
}

void exception11()
{
	kprintf("[Exception] Segment not present\n");
	hang();
}

void exception12()
{
	kprintf("[Exception] Stack segment fault\n");
	hang();
}

extern u64 x64_get_cr2();

void exception13()
{
	kprintf("[Exception] General protection fault (%x)\n",
		x64_get_cr2());
	hang();
}

void exception14()
{
	//x64_rax_n_stop(x64_get_cr2());
	kprintf("[Exception] Page fault (%x)\n",x64_get_cr2());
	//x86_print_regs();
	//Do we want to rollback in command interpreter
	
	enter_rescue_mode(0);
	while(1){__asm("hlt");}
}

void install_exceptions()
{
	idt_set_handler(0,exception0,IDT_TYPE_TRAP);
	idt_set_handler(3,exception3,IDT_TYPE_TRAP);
	idt_set_handler(6,exception6,IDT_TYPE_TRAP);
	idt_set_handler(7,exception7,IDT_TYPE_TRAP);
	idt_set_handler(8,exception8,IDT_TYPE_TRAP);
	idt_set_handler(10,exception10,IDT_TYPE_TRAP);
	idt_set_handler(11,exception11,IDT_TYPE_TRAP);
	idt_set_handler(12,exception12,IDT_TYPE_TRAP);
	idt_set_handler(13,exception13,IDT_TYPE_TRAP);
	idt_set_handler(14,exception14,IDT_TYPE_TRAP);
}
