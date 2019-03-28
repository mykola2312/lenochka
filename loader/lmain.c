#include "multiboot.h"
#include "stdlib.h"
#include "string.h"
#include "io.h"
#include "gdt.h"
#include "idt.h"
#include "exceptions.h"
#include "memory.h"
#include "elf64.h"

//1 table entry - 4KB, entire table - 2 MB
//1 dir entry - 2 MB, entire directory - 1 GB
//1 pdp entry - 1 GB, entire PDP dir - 512 GB
//1 PML4 entry - 512 GB, entire PML4 dir - 256 TB

//kernel start 0b0000000000000000 000000000 001111111 000000000 000000000 000000000000
//0000000000000000000000000001111111000000000000000000000000000000
//kernel start 0x3FC0000000

//1111111111111111111111111111111111000000000000000000000000000000

#define OS_VIRT_KERNEL64 0x1FC0000000ULL

#define PAGE_TABLE_RRESENT (1<<0)
#define PAGE_TABLE_READWRITE (1<<1)
#define PAGE_TABLE_FLAGS PAGE_TABLE_RRESENT|PAGE_TABLE_READWRITE

#define OS_MAX_MEM_GBS 128

typedef struct {
	u64 gdt_entry[8];
	
	//Paging
	u64* p_pml4; 	//[512]
	u32 res1;
	u64* p_pdp;		//[OS_MAX_MEM_GBS]
	u32 res2;
	u64* p_dir;		//[512*OS_MAX_MEM_GBS]
	u32 res3;
	u64* p_table;	//[512*512*OS_MAX_MEM_GBS]
	u32 res4;
	
	//Loader
	u64 elf_ptr;
	u64 paging_size;
	u64 multiboot;
	u64 gdt_limit;
	
	u64 kernel_phys_base;
	u64 kernel_phys_page;
	u64 kernel_phys_addr;
	u64 kernel_virt_addr;
} __packed lmem_t;

static lmem_t* lmem;// = (lmem_t*)OS_PHYS_LMEM;

static u32 base_phys_addr;

static u32 get_aligned_size(u32 size)
{
	return ((size>>12) + ((size&0xFFF) ? 1 : 0))<<12;
}

void page_map(u64 virt,u64 phys,u64 flags)
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
	
	lmem->p_table[table_idx] = (phys&0xFFFFFFFFFFFFF000)|flags;
}

void elf64_load(u32 base,u32 start,u32 end)
{
	u32 kernel_phys_addr = base;
	//At this point we need write 64 bit kernel
	Elf64_Ehdr* elf = (Elf64_Ehdr*)start;
	Elf64_Phdr* ph = (Elf64_Phdr*)((u8*)start+elf->e_phoff);
	lmem->elf_ptr = start;
	
	//Actually, out align will be always 4KB
	kprintf("ELF entry %l\n",&elf->e_entry);
	kprintf("Loading kernel at %x\n",kernel_phys_addr);
	for(u32 i = 0; i < elf->e_phnum; i++)
	{
		u32 size = get_aligned_size((u32)ph->p_memsz);
		u32 pflags = (ph->p_flags & ELF64_P_EXEC) ? PAGE_TABLE_RRESENT 
			: (PAGE_TABLE_RRESENT|PAGE_TABLE_READWRITE);
			
		kprintf("%l %l %l %l %u %u\n",&ph->p_vaddr,&ph->p_offset,
			&ph->p_memsz,&ph->p_align,ph->p_type,ph->p_flags);
		kmemzero((void*)kernel_phys_addr,size);
		if(ph->p_type == 1) //PT_LOAD
		{	
			kmemcpy((void*)kernel_phys_addr,(const void*)
				(u32)(start+ph->p_offset),(u32)ph->p_filesz);
				
			//Map
			for(u32 i = 0; i < size>>12; i++)
			{
				page_map(ph->p_vaddr+(u64)(i<<12),
					(u64)kernel_phys_addr+(u64)(i<<12),pflags);
			}
			
			kernel_phys_addr += size;
		}
			
		ph++;
	}
}

void identity_map()
{
	u64 addr = 0;
	for(u32 i = 0; i < 1024; i++)
	{
		page_map(addr,addr,PAGE_TABLE_RRESENT|PAGE_TABLE_READWRITE);
		addr += 4096;
	}
}

/*
void map_kernel(u32 base,u32 size)
{
	u64 kernel_base = lmem->kernel_virt_addr;
	u64 kernel_phys = (u64)base;
	
	kprintf("map_kernel %x %u\n",base,size);
	for(u32 i = 0; i < (size>>12); i++)
	{
		page_map(kernel_base,kernel_phys);
		kernel_base += 0x1000;
		kernel_phys += 0x1000;
	}
}
*/

void map_loader()
{
	page_map(lmem->kernel_phys_base,lmem->kernel_phys_base,
		PAGE_TABLE_RRESENT|PAGE_TABLE_READWRITE);
	u32 phys = lmem->kernel_phys_page;
	for(u32 i = 0; i < (lmem->paging_size>>12); i++)
	{
		page_map(phys,phys,PAGE_TABLE_RRESENT|PAGE_TABLE_READWRITE);
		phys += 0x1000;
	}
}

u64 gdt64_entry(u32 base,u32 limit,int is_code)
{
	u32 low,high;
	u8 access_byte;
	u8 flags;
	
	access_byte = GDT_AB_PR|GDT_AB_S|GDT_AB_RW;
	if(is_code) access_byte |= GDT_AB_EX;
	
	flags = GDT_FLAGS_GR|GDT_FLAGS_LONG;
	
	low = ((base&0xFFFF)<<16)|(limit&0xFFFF);
	high = (base>>16)&0xFF;
	high |= (access_byte&0xFF)<<8;
	high |= ((limit>>16)&0x0F)<<16;
	high |= (flags&0x0F)<<20;
	high |= ((base>>24)&0xFF)<<24;
		
	return ((u64)high<<32)|(u64)low;
}

void gdt64_setup()
{	
	lmem->gdt_entry[0] = 0;
	lmem->gdt_entry[1] = gdt64_entry(0,0xFFFFFFFF,1);
	lmem->gdt_entry[2] = gdt64_entry(0,0xFFFFFFFF,0);
	
	kprintf("CS %l DS %l\n",&lmem->gdt_entry[1],&lmem->gdt_entry[2]);
}

void scan_memory(multiboot_hdr_t* hdr)
{
	mbt_mmap_init(hdr);
	ram_map_t* map = ram_get_map();
	u32 i;
	
	for(i = 0; i < map->ram_pool_count; i++)
	{
		ram_pool_t* pool = &map->ram_pools[i];
		//We need at least 280 Megs
		kprintf("scan_memory %x\t%x\n",pool->start,pool->end);
		if((pool->end - pool->start) >= (280*1024*1024) 
			&& pool->start != 0)
		{
			// Add space so we don't override loader
			base_phys_addr = pool->start + 1048576UL;
			break;
		}
	}
}

// Initializes memory pointers
void init_memory()
{
	lmem = (lmem_t*)((u32)base_phys_addr);
	kmemzero((void*)((u32)base_phys_addr),0x1000);
	
	//lmem->kernel_phys_page = (u64)(base_phys_addr + 0x1000);
	lmem->kernel_phys_base = (u64)base_phys_addr;
	lmem->kernel_phys_page = lmem->kernel_phys_base + 0x1000ULL;
	lmem->kernel_virt_addr = OS_VIRT_KERNEL64;
	
	lmem->p_pml4 = (u64*)((u32)lmem->kernel_phys_page);
	lmem->p_pdp = (u64*)((u8*)lmem->p_pml4 
		+ get_aligned_size(512*sizeof(u64)));
	lmem->p_dir = (u64*)((u8*)lmem->p_pdp 
		+ get_aligned_size(OS_MAX_MEM_GBS*sizeof(u64)));
	lmem->p_table = (u64*)((u8*)lmem->p_dir 
		+ get_aligned_size(512*OS_MAX_MEM_GBS*sizeof(u64)));
	//kprintf("%x %x %x %x\n",lmem->p_pml4,lmem->p_pdp,
	//	lmem->p_dir,lmem->p_table);
	
	lmem->paging_size = ((u64)((u32)lmem->p_table)
		- (u64)((u32)lmem->p_pml4))
		+ (u64)(512*512*OS_MAX_MEM_GBS*sizeof(u64));
	kmemzero((void*)((u32)lmem->kernel_phys_page),lmem->paging_size);
		
	lmem->p_pml4[0] = (((u64)((u32)&lmem->p_pdp[0]))&0xFFFFFFFFFF000)|3;
	//kprintf("page->pml4[0] %x\n",&lmem->p_pml4[0]);
	
	lmem->kernel_phys_addr = lmem->kernel_phys_page 
		 + get_aligned_size(lmem->paging_size) + 0x1000ULL;
}

extern void x86enter64(u32 base);

void lmain(multiboot_hdr_t* hdr,u32 magic)
{
	outb(0x21,0xFF);
	outb(0xA1,0xFF);
	
	gdt_install();
	idt_install();
	
	chg_color(VGA_COLOR_MAGENTA);
	kprintf("PM32 LENOCHKA LOADER\n");
	kprintf("hdr %p magic %x\n",hdr,magic);
	
	scan_memory(hdr);
	init_memory();
	lmem->multiboot = (u64)hdr;
	
	install_exceptions();
	//idt_enable_interrupts();
	
	multiboot_module_t* mod = (multiboot_module_t*)hdr->mods_addr;
	for(u32 i = 0; i < hdr->mods_count; i++)
	{
		kprintf("%s\t%p\t%p\t%s\n",(const char*)mod[i].cmd_line,
			mod[i].mod_start,mod[i].mod_end,
			mod[i].mod_start);
		if(!kstrncmp((const char*)mod[i].cmd_line,"LENOCHKA"))
			break;
	}
	kprintf("\nLENOCHKA MOD %p\n",mod);
	kprintf("page tables size %x\n",lmem->paging_size);
	
	elf64_load((u32)lmem->kernel_phys_addr,
		mod->mod_start,mod->mod_end);
	identity_map();
	map_loader();
	//map_kernel((u32)lmem->kernel_phys_addr,size);
	gdt64_setup();
	
	kprintf("entry %l %x\n",&((Elf64_Ehdr*)((u32)lmem->elf_ptr))->e_entry);
	
	kprintf("base_phys_addr %x\nkernel_phys_page %l\nkernel_phys_addr %l\n",
		base_phys_addr,&lmem->kernel_phys_page,&lmem->kernel_phys_addr);
	
	/*kprintf("lmem->p_pml4 %x\n",lmem->p_pml4);
	kprintf("lmem->p_pdp %x\n",lmem->p_pdp);
	kprintf("lmem->p_dir %x\n",lmem->p_dir);
	kprintf("lmem->p_table %x\n",lmem->p_table);
	kprintf("lmem->kernel_phys_base %l\n",&lmem->kernel_phys_base);
	kprintf("lmem->kernel_phys_page %l\n",&lmem->kernel_phys_page);
	kprintf("lmem->kernel_phys_addr %l\n",&lmem->kernel_phys_addr);
	kprintf("lmem->kernel_virt_addr %l\n",&lmem->kernel_virt_addr);*/
	
	kprintf("lmem_t %u\n",sizeof(lmem_t));
	
	//kprintf("%u\n",(u32)(&((Elf64_Ehdr*)lmem->elf_ptr)->e_entry)-(u32)lmem->elf_ptr);
	
	//ENTER
	//while(1){__asm("hlt");}
	x86enter64(base_phys_addr);
}
