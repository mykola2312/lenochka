#include "pic.h"

void pic_init(int master,int slave)
{
	outb(PIC1,ICW1);
	outb(PIC2,ICW1);
	
	outb(PIC1+1,master&0xFF);
	outb(PIC2+1,slave&0xFF);
	
	outb(PIC1+1,4);
	outb(PIC2+1,2);
	
	outb(PIC1+1,ICW4);
	outb(PIC2+1,ICW4);
	
	//Disable all IRQs
	outb(PIC1+1,0xFF);
	outb(PIC2+1,0xFF);
}

void pic_set_mask(int irq,int enabled)
{
	u16 pic;
	u8 mask;
	
	if(irq >= 8)
	{
		pic = PIC2+1;
		irq -= 8;
	}
	else pic = PIC1+1;
	
	mask = inb(pic);
	if(enabled) mask &= ~(1<<irq);
	else mask |= (1<<irq);
	
	outb(pic,mask);
}

void pic_send_eoi(int irq)
{
	if(irq >= 8) outb(PIC2,0x20);
	outb(PIC1,0x20);
}
