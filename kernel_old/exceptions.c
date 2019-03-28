#include "exceptions.h"
#include "vga_terminal.h"
#include "termlib.h"
#include "idt.h"
#include "pit.h"
#include "x86utils.h"

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
	x86_print_regs();
	while(1){__asm("hlt");}
}

extern void command_interpreter();
extern void _load_legacy_stack();

void _enter_rescue_mode()
{
	_load_legacy_stack();
	sleep(500);
	command_interpreter();
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

void exception0(int idx)
{
	kprintf("[Exception] Division by zero\n");
	hang();
}

void exception3(int idx)
{
	kprintf("[Exception] Debug\n");
	hang();
}

void exception6(int idx)
{
	kprintf("[Exception] Invalid opcode\n");
	hang();
}

void exception7(int idx)
{
	kprintf("[Exception] Device not available\n");
	hang();
}

void exception8(int idx)
{
	kprintf("[Exception] Double fault\n");
	hang();
}

void exception10(int idx)
{
	kprintf("[Exception] Invalid TSS\n");
	hang();
}

void exception11(int idx)
{
	kprintf("[Exception] Segment not present\n");
	hang();
}

void exception12(int idx)
{
	kprintf("[Exception] Stack segment fault\n");
	hang();
}

void exception13(int idx)
{
	kprintf("[Exception] General protection fault\n");
	hang();
}

void exception14(int idx)
{
	kprintf("[Exception] Page fault\n");
	x86_print_regs();
	//Do we want to rollback in command interpreter
	
	enter_rescue_mode(0);
	while(1){__asm("hlt");}
}

void install_exceptions()
{
	idt_setup(_int0_handler,exception0,0,IDT_TYPE_TRAP);
	idt_setup(_int3_handler,exception3,3,IDT_TYPE_TRAP);
	idt_setup(_int6_handler,exception6,6,IDT_TYPE_TRAP);
	idt_setup(_int7_handler,exception7,7,IDT_TYPE_TRAP);
	idt_setup(_int8_handler,exception8,8,IDT_TYPE_TRAP);
	//idt_setup(_int9_handler,exception9,9,IDT_TYPE_TRAP);
	idt_setup(_int10_handler,exception10,10,IDT_TYPE_TRAP);
	idt_setup(_int11_handler,exception11,11,IDT_TYPE_TRAP);
	idt_setup(_int12_handler,exception12,12,IDT_TYPE_TRAP);
	idt_setup(_int13_handler,exception13,13,IDT_TYPE_TRAP);
	idt_setup(_int14_handler,exception14,14,IDT_TYPE_TRAP);
}
