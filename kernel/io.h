#ifndef __IO_H
#define __IO_H

#include "os.h"

extern u8 inb(u16 port);
extern void outb(u16 port,u8 data);

extern u16 inw(u16 port);
extern void outw(u16 port,u16 data);

extern u32 inl(u16 port);
extern void outl(u16 port,u32 data);

extern void _insw(u16 port,u8* buffer,u32 count);
extern void _outsw(u16 port,u8* buffer,u32 count);

extern void _insw256(u16 port,u8* buffer);
extern void _outsw256(u16 port,u8* buffer);

#endif
