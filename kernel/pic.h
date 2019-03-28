#ifndef __PIC_H
#define __PIC_H

#include "io.h"

#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01

void pic_init(int master,int slave);
void pic_set_mask(int irq,int enabled);
void pic_send_eoi(int irq);

#endif
