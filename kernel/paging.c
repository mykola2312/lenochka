#include "paging.h"
#include "defines.h"

//1 table entry - 4KB, entire table - 2 MB
//1 dir entry - 2 MB, entire directory - 1 GB
//1 pdp entry - 1 GB, entire PDP dir - 512 GB
//1 PML4 entry - 512 GB, entire PML4 dir - 256 TB

/*
void page_map(u64 virt,u64 phys)
{
	u32 table_idx,dir_idx,pdp_idx;
	u64 addr;
	
	table_idx = (virt>>12) & 0x7FFFFFF;
	dir_idx = table_idx>>9;
	pdp_idx = dir_idx>>9;
	
	if(!(lmem->p_pdp[pdp_idx] & 3))
	{
		addr = (u64)((u32)&lmem->p_dir[dir_idx]);
		//kprintf("addr %l\n",&addr);
		lmem->p_pdp[pdp_idx] = (addr&0xFFFFFFFFFFFFF000)|3;
	}
	
	if(!(lmem->p_dir[dir_idx] & 3))
	{
		addr = (u64)((u32)&lmem->p_table[table_idx]);
		lmem->p_dir[dir_idx] = (addr&0xFFFFFFFFFFFFF000)|3;
	}
	
	lmem->p_table[table_idx] = (phys&0xFFFFFFFFFFFFF000)|3;
}*/

//	RESERVED		PML4		PDP		DIR			TABLE	OFFSET
//0000000000000000 000000000 001111111 000000000 000000000 000000000000

void page_map(void* __virt virt,void* __phys phys,size_t size)
{
	u64 table_idx,dir_idx,pdp_idx;
	u64 table_cnt,dir_cnt,pdp_cnt;
	u64 vaddr,paddr;
	
	vaddr = (u64)virt;
	paddr = (u64)phys;
	
	table_idx = (vaddr>>12) & 0x7FFFFFF;
	dir_idx = table_idx>>9;
	pdp_idx = dir_idx>>9;
	
	table_cnt = ROUND2_UP(size,12) >> 12;
	dir_cnt = ROUND1_ADD((table_cnt>>9));
	pdp_cnt = ROUND1_ADD((dir_cnt>>9));
	
	for(u64 i = 0; i < table_cnt; i++)
	{
		lmem->p_table[table_idx+i] = 
			((paddr+(i<<12))&0xFFFFFFFFFFFFF000) | PAGE_TABLE_FLAGS;
	}
	
	for(u64 i = 0; i < dir_cnt; i++)
	{
		u64 addr;
		addr = (u64)(&lmem->p_table[(dir_idx+i)<<9]);
		lmem->p_dir[dir_idx+i] = (addr&0xFFFFFFFFFFFFF000) | PAGE_TABLE_FLAGS;
	}
	
	for(u64 i = 0; i < pdp_cnt; i++)
	{
		u64 addr;
		addr = (u64)(&lmem->p_dir[(pdp_idx+i)<<9]);
		lmem->p_pdp[pdp_idx+i] = (addr&0xFFFFFFFFFFFFF000) | PAGE_TABLE_FLAGS;
	}
}

void page_unmap(void* __virt virt,size_t size)
{
	u64 table_idx,dir_idx,pdp_idx;
	u64 table_cnt,dir_cnt,pdp_cnt;
	u64 vaddr;
	
	vaddr = (u64)virt;
	
	table_idx = (vaddr>>12) & 0x7FFFFFF;
	dir_idx = table_idx>>9;
	pdp_idx = dir_idx>>9;
	
	table_cnt = ROUND2_UP(size,12) >> 12;
	dir_cnt = ROUND1_ADD((table_cnt>>9));
	pdp_cnt = ROUND1_ADD((dir_cnt>>9));
	
	for(u64 i = 0; i < table_cnt; i++)
		lmem->p_table[table_idx+i] = 0;
	
	for(u64 i = 0; i < dir_cnt; i++)
	{
		u64 j;
		//Check if directory empty
		for(j = 0; j < 512; j++)
		{
			if(lmem->p_table[((dir_idx+i)<<9)+j] & PAGE_TABLE_FLAGS)
				break;
		}
		//Empty
		if(j == 512) lmem->p_dir[dir_idx+i] = 0;
	}
	
	for(u64 i = 0; i < pdp_cnt; i++)
	{
		u64 j;
		//Check if PDP empty
		for(j = 0 ; j < 512; j++)
		{
			if(lmem->p_dir[((pdp_idx+i)<<9)+j] & PAGE_TABLE_FLAGS)
				break;
		}
		//Empty
		if(j == 512) lmem->p_pdp[pdp_idx+i] = 0;
	}
}

u64 __phys page_v2p(void* __virt virt)
{
	u64 table_idx,page;
	
	table_idx = (((u64)virt)>>12) & 0x7FFFFFF;
	page = lmem->p_table[table_idx];
	
	if(page & PAGE_TABLE_FLAGS) return page & 0xFFFFFFFFFFFFF000;
	return -1;
}
