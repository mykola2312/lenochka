#include "lvalloc.h"
#include "paging.h"
#include "defines.h"
#include "stdlib.h"

void* lvalloc(size_t size)
{
	u64 addr;
	
	addr = (u64)lvalloc_get_next_addr();
	page_map((void*)addr,(void*)addr,size);

	lmem->lv_current += (ROUND2_UP(size,12) >> 12);
	
	kmemzero((void*)addr,ROUND2_UP(size,12));
	return (void*)addr;
}

void* lvalloc_get_next_addr()
{
	return (void*)((u64)lmem->lv_start+(lmem->lv_current<<12));
}

void lvalloc_push()
{
	lmem->lv_stack[--lmem->lv_sp] = lmem->lv_current;
}

void lvalloc_pop()
{
	lmem->lv_current = lmem->lv_stack[lmem->lv_sp++];
}

void lvalloc_init()
{
	lmem->lv_sp = MAX_LV_SSIZE;
	lmem->lv_start = (u8*)(lmem->kernel_phys_addr + KERNEL_SIZE);
}
