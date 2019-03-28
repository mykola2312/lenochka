#include "paging.h"
#include "termlib.h"

// Allocate some space for page directories & tables
//static u32 page_directories[PAGE_DIR_NUM] __align(4096);
//static u32 page_tables[PAGE_TABLE_NUM] __align(4096);

void page_map(u32 __virt virt,u32 __phys phys,u32 size,u32 flags)
{
	u32 num_dirs,num_pages;
	u32 dir_index,table_index;
	u32 i;
	
	num_pages = (size>>12) + ((size&0xFFF) ? 1 : 0);
	num_dirs = (num_pages>>10) + ((num_pages&0x3FFF) ? 1 : 0);
	
	table_index = virt>>12;
	dir_index = table_index>>10;
	
	//Make directories present
	for(i = 0; i < num_dirs; i++)
	{
		if(!(page_directories[dir_index+i] & PAGE_DIR_PRESENT))
		{
			page_directories[dir_index+i] = PAGE_DIR_ENTRY(
				OS_PHYS_ADDR( (u32)(&page_tables[(dir_index+i)<<10]) ),
				PAGE_DIR_PRESENT
				|	PAGE_DIR_READWRITE
			);
		}
	}
	
	//Make tables present
	for(i = 0; i < num_pages; i++)
	{
		page_tables[table_index+i] = PAGE_TABLE_ENTRY(
			(u32)(phys+i*4096),
			flags|PAGE_TABLE_PRESENT
		);
	}
}

extern void x86_reload_cr3();

void page_unmap(u32 __virt virt,u32 size)
{
	u32 num_dirs,num_pages;
	u32 dir_index,table_index;
	u32 i,reload_cr3;
	
	num_pages = (size>>12) + ((size&0xFFF) ? 1 : 0);
	num_dirs = (num_pages>>10) + ((num_pages&0x3FFF) ? 1 : 0);
	
	table_index = virt>>12;
	dir_index = table_index>>10;
	
	for(i = 0; i < num_pages; i++)
		page_tables[table_index+i] = 0;
	
	//Check directories
	reload_cr3 = 0;
	for(i = 0; i < num_dirs; i++)
	{
		u32 j;
		
		for(j = 0; j < 1024; j++)
		{
			if(page_tables[((dir_index+i)<<10) + j] & PAGE_TABLE_PRESENT)
				break;
		}
		
		//Directory empty
		if(j == 1024)
		{
			page_directories[dir_index+i] = 0;
			reload_cr3 = 1;
		}
	}
	if(reload_cr3) x86_reload_cr3();
}

__phys u32 page_v2p(u32 __virt virt)
{
	u32 index,dir_entry;
	u32* table;
	
	index = virt>>12;
	dir_entry = page_directories[index>>10];
	if(!(dir_entry & PAGE_DIR_PRESENT)) return 0;
	table = (u32*)((dir_entry&0xFFFFF000) + OS_VIRT_MAPPING);
	
	return table[index&0x3FF] & 0xFFFFF000;
}
