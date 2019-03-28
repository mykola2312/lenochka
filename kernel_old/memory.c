#include "memory.h"

static ram_map_t s_RAMmap;

void mbt_mmap_init(multiboot_hdr_t* mbt)
{
	s_RAMmap.ram_total_size = 0;
	s_RAMmap.ram_pool_count = 0;
	multiboot_mmap_t* mmap = (multiboot_mmap_t*)mbt->mmap_addr;
	while((u32)mmap < (u32)mbt->mmap_addr + mbt->mmap_length)
	{
		if(mmap->type == 1)
		{
			ram_pool_t* pool;
			if(s_RAMmap.ram_pool_count == MAX_RAM_POOLS)
				break;
			pool = &s_RAMmap.ram_pools[s_RAMmap.ram_pool_count++];
			
			pool->start = (u32)mmap->base_addr;
			pool->end = pool->start + (u32)mmap->length;
			
			s_RAMmap.ram_total_size += mmap->length;
		}
		mmap = (multiboot_mmap_t*)((u32)mmap + mmap->size + 4);
	}
}

ram_map_t* ram_get_map()
{
	return &s_RAMmap;
}

u32 ram_get_size()
{
	return s_RAMmap.ram_total_size;
}
