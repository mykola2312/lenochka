#ifndef __IDT_H
#define __IDT_H

#include "os.h"

#define IDT_TYPE_INTR 0x0E
#define IDT_TYPE_TRAP 0x0F

typedef void (*inthandler_t)();

void idt_set_handler(u8 idx,inthandler_t hndr,u8 type);
void idt_call_handler(u8 idx);

void idt_install();
void idt_enable_interrupts();

#endif
