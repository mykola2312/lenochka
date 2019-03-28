#ifndef __STDLIB_H
#define __STDLIB_H

#include "os.h"
#include "vga_terminal.h"
#include "string.h"

//stdlib.h

extern void kmemcpy(void* dst,const void* src,size_t len);
extern int kmemcmp(const void* dst,const void* src,size_t len);
extern void kmemzero(void* dst,size_t len);

//termlib.h
#define kputc printchar

void iprint(i64 number);
void uprint(u64 number);
void xprint(u64 number);

void iprints(char* str,i64 number);
void uprints(char* str,u64 number);
void xprints(char* str,u64 number);

void kprint(char* str);

void ksprintf(char* dst,size_t maxLen,const char* fmt,...);
void kprintf(const char* fmt,...);

//termio.h

u8 kgetch(); //Wait for key scan code
dbcs_t kgetc(); //Wait for ASCII character

void kgets(dbcs_t* buf,size_t cMax);

i64 katoi(char* str,int base);

void ksscanf(char* buf,char* fmt,...);

#define _debug()									\
	kprintf("_debug at %s:%u\n",__FILE__,__LINE__);	\
	kgetc();

#endif
