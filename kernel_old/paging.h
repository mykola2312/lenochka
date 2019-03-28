#ifndef __PAGING_H
#define __PAGING_H

#include "os.h"

#define PAGE_DIR_PRESENT 	(1<<0)
#define PAGE_DIR_READWRITE 		(1<<1)
#define PAGE_DIR_USERMODE 		(1<<2)
#define PAGE_DIR_WRITECACHE		(1<<3)
#define PAGE_DIR_NOCACHE 		(1<<4)
#define PAGE_DIR_ACCESSED 		(1<<5)
#define PAGE_DIR_PAGESIZE 		(1<<7)

#define PAGE_DIR_ENTRY(phys,flags) ((phys&0xFFFFF000)|flags)

#define PAGE_TABLE_PRESENT 		(1<<0)
#define PAGE_TABLE_READWRITE 	(1<<1)
#define PAGE_TABLE_USERMODE 	(1<<2)
#define PAGE_TABLE_WRITECACHE 	(1<<3)
#define PAGE_TABLE_CACHE 		(1<<4)
#define PAGE_TABLE_GLOBAL 		(1<<8)

#define PAGE_TABLE_ENTRY(phys,flags) ((phys&0xFFFFF000)|flags)

#define PAGE_DIR_NUM 1024
#define PAGE_TABLE_NUM OS_PM86_MEM_SIZE_4K

//Map physical address to virtual
void page_map(u32 __virt virt,u32 __phys phys,u32 size,u32 flags);

void page_unmap(u32 __virt virt,u32 size);
__phys u32 page_v2p(u32 __virt virt);

extern u32 page_directories[PAGE_DIR_NUM];
extern u32 page_tables[PAGE_TABLE_NUM];

#endif
