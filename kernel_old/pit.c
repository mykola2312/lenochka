#include "pit.h"
#include "pic.h"
#include "idt.h"

static u32 s_counter = 0;

void timer_interrupt(int idx)
{
	if(s_counter) s_counter--;
	pic_send_eoi(0);
}

void sleep(u32 msecs)
{
	__asm("cli");
	if(msecs < 55) msecs = 55;
	s_counter = msecs/55;
	__asm("sti");
	//pic_set_mask(0,1);
	do {
		__asm("hlt"); //Wait for IRQ
	} while(s_counter);
	//pic_set_mask(0,0);
}

void pit_init()
{
	//Setup IRQ
	pic_set_mask(0,1);
	idt_setup(_int15_handler,timer_interrupt,32,IDT_TYPE_INTERRUPT);
	
	//Init PIT
	outb(0x43,0x36);
	outb(0x40,0x00);
	outb(0x40,0x00);
}
