#ifndef __GDT_H
#define __GDT_H

#include "os.h"

#define GDT_AB_PR 		(1<<7)
#define GDT_AB_PRIVL 	(1<<5)
#define GDT_AB_S		(1<<4)
#define GDT_AB_EX		(1<<3)
#define GDT_AB_DC		(1<<2)
#define GDT_AB_RW		(1<<1)
#define GDB_AB_AC		(1<<0)

#define GDT_FLAGS_GR	(1<<3)
#define GDT_FLAGS_SZ	(1<<2)

enum {
	GDT_NULL = 0,
	GDT_CODE,
	GDT_DATA,
	GDT_MAX,
};

u64 gdt_create(u32 base,u32 limit,u32 type);
void gdt_install();

extern void _gdt_install(void* gdt,int size);

#endif
