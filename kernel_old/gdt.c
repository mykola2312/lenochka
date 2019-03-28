#include "gdt.h"

static u64 gdt_table[GDT_MAX];

u64 gdt_create(u32 base,u32 limit,u32 type)
{
	u64 desc;
	u8 access_byte;
	u8 flags;
	
	access_byte = GDT_AB_PR|GDT_AB_S|GDT_AB_RW;
	if(type == GDT_CODE)
	{
		access_byte |= GDT_AB_EX;
	}
	
	flags = GDT_FLAGS_GR|GDT_FLAGS_SZ;
	
	desc = 0;
	desc |= (u64)(limit & 0xFFFF)			<< 0;
	desc |= (u64)(base & 0xFFFF) 			<< 16;
	desc |= (u64)((base>>16) & 0xFF) 		<< 32;
	desc |= (u64)(access_byte & 0xFF)	 	<< 40;
	desc |= (u64)((limit>>16) & 0x0F) 		<< 48;
	desc |= (u64)(flags & 0x0F) 			<< 52;
	desc |= (u64)((base>>24) & 0xFF) 		<< 56;
	
	return desc;
}

void gdt_install()
{
	// Protected Mode Flat Model
	gdt_table[GDT_NULL] = gdt_create(0,0,0);
	gdt_table[GDT_CODE] = gdt_create(0,0x000FFFFF,GDT_CODE);
	gdt_table[GDT_DATA] = gdt_create(0,0x000FFFFF,GDT_DATA);
	
	_gdt_install(gdt_table,sizeof(gdt_table)-1);
}
