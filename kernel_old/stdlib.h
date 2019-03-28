#ifndef __STDLIB_H
#define __STDLIB_H

#include "os.h"
#include "vga_terminal.h"

//stdlib.h

extern void kmemcpy(void* dst,const void* src,u32 len);
extern int kmemcmp(const void* dst,const void* src,u32 len);
extern void kmemzero(void* dst,u32 len);

//termlib.h
#define kputc printchar

void iprint(int number);
void uprint(u32 number);
void xprint(u32 number);

void iprints(char* str,int number);
void uprints(char* str,u32 number);
void xprints(char* str,u32 number);

void kprint(char* str);

void ksprintf(char* dst,u32 maxLen,const char* fmt,...);
void kprintf(const char* fmt,...);

//termio.h

u8 kgetch(); //Wait for key scan code
u8 kgetc(); //Wait for ASCII character

void kgets(char* buf,size_t max);

i32 katoi(char* str,int base);

void ksscanf(char* buf,char* fmt,...);

#define _debug()									\
	kprintf("_debug at %s:%u\n",__FILE__,__LINE__);	\
	kgetc();

#endif
