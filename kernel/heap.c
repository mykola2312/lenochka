#include "heap.h"
#include "defines.h"
#include "lvalloc.h"
#include "paging.h"
#include "stdlib.h"

#define MBLOCK_ATTR_ALLOC (1<<0)

#define IS_MBLOCK_ALLOC(mblock) (mblock->size & MBLOCK_ATTR_ALLOC)

//static mheap_t s_heap; //heap for all available RAM

/*void heap_init(mheap_t* heap,u64 size)
{
	heap->heapsz = size>>21;
	heap->heap = (u64*)mmap(NULL,(heap->heapsz<<21),
		PROT_READ|PROT_WRITE,
		MAP_ANONYMOUS|MAP_PRIVATE,
		-1,
		0
	);
	memset(heap->heap,0,(heap->heapsz<<21));
}*/

static mblock_t* mblock_get_next(mblock_t* block)
{
	return (mblock_t*)((u8*)block+(block->size & ~7));
}

static mblock_t* mblock_by_data(void* data)
{
	return (mblock_t*)((u8*)data-24);
}

static u64 mblock_get_size(mblock_t* block)
{
	return block->size & ~7;
}

void heap_join(mblock_t* start,int dir)
{
	mblock_t* cur;
	u64 blk_size;
	
	blk_size = 0;
	cur = start;
	while(cur != NULL && !IS_MBLOCK_ALLOC(cur))
	{
		blk_size += mblock_get_size(cur);
		cur->size = 0;
		if(dir > 0) cur = cur->next;
		else if(dir < 0) cur = cur->prev;
	}
	
	if(dir > 0)
	{
		start->next = cur;
		start->size = blk_size & ~7;
		if(cur)
		{
			if(cur->next) cur->next->prev = start;
		}
	}
	else if(dir < 0)
	{
		if(cur)
		{
			cur = cur->next;
			cur->size = blk_size & ~7;
			cur->next = start->next;
		}
	}
}

void heap_split(mblock_t* block,u64 req_size)
{
	u64 tot_size = mblock_get_size(block);
	mblock_t* split;
	
	block->size = (req_size)|MBLOCK_ATTR_ALLOC;
	split = mblock_get_next(block);
	
	//Init prev and next
	split->prev = block;
	split->next = block->next;
	block->next = split;
	
	split->size = (tot_size-req_size) & ~7;
}

void* heap_alloc(mheap_t* heap,u64 size)
{
	mblock_t* start,*block,*prev;
	size = ROUND2_UP((size+24),5);
	
	/*for(i = 0; i < heap->heapsz; i++)
	{
		start = (mblock_t*)((u8*)heap->heap+(i<<21));
		if(!(start->size & MBLOCK_ATTR_ALLOC_2MB)) break;
	}
	if(i == heap->heapsz) return NULL;*/
	start = (mblock_t*)heap->heap;
	
	//Find free block
	prev = NULL;
	block = start;
	do {
		if((!IS_MBLOCK_ALLOC(block) && mblock_get_size(block) >= size)
			|| block->size == 0)
			break;
		prev = block;
		if(block->next) block = block->next;
		else break;
	} while(block);
	if(prev == block) prev = NULL;
	//Allocate
	if(block->size == 0) //Unformatted
	{
		block->size = size|MBLOCK_ATTR_ALLOC;
		block->prev = prev;
		block->next = mblock_get_next(block);
	}
	else heap_split(block,size);
	
	if(prev && !prev->next) prev->next = block;
	
	return block->data;
}

void heap_free(mheap_t* heap,void* mem)
{
	mblock_t* block = (mblock_t*)((u8*)mem-24);
	(void)heap;
	
	//Unlink
	block->size &= ~MBLOCK_ATTR_ALLOC;
	//Link free blocks prev and next
	heap_join(block,1);
	heap_join(block,-1);
}

void heap_init()
{
	ram_pool_t* pool;
	u64 addr = (u64)lvalloc_get_next_addr();
	u64 i;
	
	//Decide in which mapping we have lvalloc mapped and kernel
	for(i = 0; i < lmem->ram_pools; i++)
	{
		pool = &lmem->ram_pool[i];
		if(addr >= pool->start && addr < pool->end)
			break;
	}
	if(i == lmem->ram_pools)
	{
		kprintf("heap_init failed!\n");
		return;
	}
	
	lmem->heap.heap = (u64*)addr;
	lmem->heap.heapsz = (((pool->end - addr)>>21) - 1);
	
	kprintf("heap %p heapsz %u\n",lmem->heap.heap,lmem->heap.heapsz<<21);
	
	//Identity page map
	//page_map(s_heap.heap,s_heap.heap,s_heap.heapsz<<21);
	for(u64 i = 0; i < (lmem->heap.heapsz<<21)>>12; i++)
	{
		page_map((u8*)lmem->heap.heap+(i<<12),(u8*)lmem->heap.heap+(i<<12),
			4096);
	}
	
	kmemzero(lmem->heap.heap,lmem->heap.heapsz<<21);
}

void* kmalloc(u64 size)
{
	return heap_alloc(&lmem->heap,size);
}

void* krealloc(void* addr,u64 newsize)
{
	void* newaddr = kmalloc(newsize);
	if(!newaddr) return NULL;
	kmemcpy(newaddr,addr,mblock_get_size(mblock_by_data(addr))-24);
	kfree(addr);
	return newaddr;
}

void kfree(void* addr)
{
	heap_free(&lmem->heap,addr);
}
