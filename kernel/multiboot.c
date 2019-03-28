#include "os.h"
#include "multiboot.h"
#include "stdlib.h"

void mbt_scan_memory()
{
	lmem->ram_size = 0;
	lmem->ram_pools = 0;
	
	multiboot_hdr_t* mbt = (multiboot_hdr_t*)((u64)lmem->multiboot);
	multiboot_mmap_t* mmap = (multiboot_mmap_t*)((u64)mbt->mmap_addr);
	
	while((u64)mmap < mbt->mmap_addr + mbt->mmap_length)
	{
		if(mmap->type == 1)
		{
			ram_pool_t* pool;
			if(lmem->ram_pools == MAX_RAM_POOLS)
				break;
			pool = &lmem->ram_pool[lmem->ram_pools++];
			
			pool->start = mmap->base_addr;
			pool->end = mmap->base_addr + mmap->length;
			
			lmem->ram_size += mmap->length;
		}
		mmap = (multiboot_mmap_t*)((u64)mmap + mmap->size + 4);
	}
	
	kprintf("RAM size %f\n",((lmem->ram_size>>20)<<16)>>10);
	for(u64 i = 0; i < lmem->ram_pools; i++)
	{
		kprintf("RAM %x - %x\n",lmem->ram_pool[i].start,
			lmem->ram_pool[i].end);
	}
}
