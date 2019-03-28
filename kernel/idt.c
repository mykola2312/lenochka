#include "idt.h"
#include "stdlib.h"

extern void int0_handler();
extern void int1_handler();
extern void int2_handler();
extern void int3_handler();
extern void int4_handler();
extern void int5_handler();
extern void int6_handler();
extern void int7_handler();
extern void int8_handler();
extern void int9_handler();
extern void int10_handler();
extern void int11_handler();
extern void int12_handler();
extern void int13_handler();
extern void int14_handler();
extern void int15_handler();
extern void int16_handler();
extern void int17_handler();
extern void int18_handler();
extern void int19_handler();
extern void int20_handler();
extern void int21_handler();
extern void int22_handler();
extern void int23_handler();
extern void int24_handler();
extern void int25_handler();
extern void int26_handler();
extern void int27_handler();
extern void int28_handler();
extern void int29_handler();
extern void int30_handler();
extern void int31_handler();

static u32 idt_table[1024];
static inthandler_t inthandlers[256];
static inthandler_t handlers[32] = {
	int0_handler,int1_handler,int2_handler,int3_handler,
	int4_handler,int5_handler,int6_handler,int7_handler,
	int8_handler,int9_handler,int10_handler,int11_handler,
	int12_handler,int13_handler,int14_handler,int15_handler,
	int16_handler,int17_handler,int18_handler,int19_handler,
	int20_handler,int21_handler,int22_handler,int23_handler,
	int24_handler,int25_handler,int26_handler,int27_handler,
	int28_handler,int29_handler,int30_handler,int31_handler
};
static u64 cur_handler = 0;

void idt_set_handler(u8 idx,inthandler_t hndr,u8 type)
{
	u32* item = (u32*)(&idt_table[idx<<2]);
	u64 addr;
	
	if(cur_handler == 32)
	{
		kprintf("idt_set_handler: no free handlers!\n");
		return;
	}
	inthandlers[cur_handler] = hndr;
	addr = (u64)handlers[cur_handler++];
	
	item[0] = (OS_CODE_SELECTOR<<16)|(addr&0xFFFF);
	item[1] = (((addr>>16)&0xFFFF)<<16)|(1<<15)|((type&0x0F)<<8);
	item[2] = (addr>>32);
	item[3] = 0;
	
}

void idt_call_handler(u8 handler)
{
	if(inthandlers[handler])
	{
		inthandlers[handler]();
	}
}

extern void _idt_install(void* table,u64 limit);
void idt_install()
{
	kmemzero(idt_table,sizeof(idt_table));
	kmemzero(inthandlers,sizeof(inthandlers));
	
	_idt_install(idt_table,sizeof(idt_table));
}

void idt_enable_interrupts()
{
	__asm("sti");
}
