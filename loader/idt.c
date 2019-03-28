#include "idt.h"
#include "stdlib.h"

static u64 idt_table[256]; //256 entries lol
static idt_handler idt_handlers[256];

u64 idt_create(void* handler,int type)
{
	u64 desc;
	u32 addr;
	
	addr = (u32)handler;
	
	desc = 0;
	desc |= (u64)(addr&0xFFFF)				<< 0;
	desc |= (u64)(OS_CODE_SELECTOR&0xFFFF) 	<< 16;
	desc |= (u64)(type&0x0F)				<< 40;
	desc |=	(u64)(1)						<< 47;
	desc |= (u64)(addr>>16)					<< 48;
	
	return desc;
}

void idt_setup(void* int_handler,idt_handler handler,int idx,int type)
{
	idt_table[idx] = idt_create(int_handler,type);
	idt_handlers[idx] = handler;
}

static void* idt_get_addr(int i)
{
	return (void*)(((u32)((idt_table[i]>>48)<<16)|(u32)(idt_table[i]&0xFFFF)));
}

int idt_irq_by_addr(void* addr)
{
	int i;
	
	for(i = 0; i < 256; i++)
		if(idt_get_addr(i) == addr) return i;
	
	return -1;
}

void idt_handler_entry(void* addr)
{
	int idx;
	
	idx = idt_irq_by_addr(addr);
	if(idx == -1)
	{
		kprintf("No IDT entry some INT!\n");
		return;
	}
	
	//Call handler
	idt_handlers[idx](idx);
}

void idt_install()
{
	int i;
	
	for(i = 0; i < 256; i++) idt_table[i] = 0;
	
	_idt_install(idt_table,sizeof(idt_table)-1);
}

void idt_enable_interrupts()
{
	__asm("sti");
}
