#ifndef __IDT_H
#define __IDT_H

#include "os.h"

typedef enum {
	IDT_TYPE_TASK = 5,
	IDT_TYPE_16_INTERRUPT = 6,
	IDT_TYPE_16_TRAP = 7,
	IDT_TYPE_INTERRUPT = 14,
	IDT_TYPE_TRAP = 15,
} idt_type;

typedef void (*idt_handler)(int irq);

u64 idt_create(void* handler,int type);
void idt_setup(void* int_handler,idt_handler handler,int idx,int type);
int idt_irq_by_addr(void* addr);

void idt_handler_entry(void* addr);

void idt_install();
extern void _idt_install(void* idt,int limit);

void idt_enable_interrupts();

extern void _int0_handler();
extern void _int1_handler();
extern void _int2_handler();
extern void _int3_handler();
extern void _int4_handler();
extern void _int5_handler();
extern void _int6_handler();
extern void _int7_handler();
extern void _int8_handler();
extern void _int9_handler();
extern void _int10_handler();
extern void _int11_handler();
extern void _int12_handler();
extern void _int13_handler();
extern void _int14_handler();
extern void _int15_handler();
extern void _int16_handler();
extern void _int17_handler();
extern void _int18_handler();
extern void _int19_handler();
extern void _int20_handler();
extern void _int21_handler();
extern void _int22_handler();
extern void _int23_handler();
extern void _int24_handler();
extern void _int25_handler();
extern void _int26_handler();
extern void _int27_handler();
extern void _int28_handler();
extern void _int29_handler();
extern void _int30_handler();
extern void _int31_handler();

#endif
