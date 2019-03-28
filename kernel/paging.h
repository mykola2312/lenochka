#ifndef __PAGING_H
#define __PAGING_H

#include "os.h"

#define PAGE_TABLE_RRESENT (1<<0)
#define PAGE_TABLE_READWRITE (1<<1)
#define PAGE_TABLE_FLAGS (PAGE_TABLE_RRESENT|PAGE_TABLE_READWRITE)

void page_map(void* __virt virt,void* __phys phys,size_t size);
void page_unmap(void* __virt virt,size_t size);
u64 __phys page_v2p(void* __virt virt);

#endif
