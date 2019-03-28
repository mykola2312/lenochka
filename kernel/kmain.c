#include "os.h"
#include "stdlib.h"
#include "string.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "pci.h"
#include "exceptions.h"
#include "keyboard.h"
#include "paging.h"
#include "lvalloc.h"
#include "multiboot.h"
#include "disk.h"
#include "heap.h"
#include "file.h"
#include "fat.h"
#include "video.h"

// IDT
// PIC
// PIT
// paging
// keyboard

static char* xnums = "0123456789ABCDEF";

lmem_t* lmem;
disk_t* disk1;

void command_interpreter()
{
	dbcs_t _command[64];
	char command[128];
	char name[512];
	char* buf = (char*)kmalloc(4096);

	while(1)
	{
		kputc('>');
		kgets(_command,64);
		kdbcs2str(_command,command,sizeof(command));
		/*if(!kstrncmp(command,"ram_map"))
		{
			ram_map_t* ram = ram_get_map();
			kprintf("RAM size = %u\nRAM pools %u\n",
				ram->ram_total_size,ram->ram_pool_count);
			for(u32 i = 0; i < ram->ram_pool_count; i++)
			{
				kprintf("\t%p\t%p\n",ram->ram_pools[i].start,
					ram->ram_pools[i].end);
			}
		}
		else if(!kstrncmp(command,"page_map"))
		{
			u32 virt,phys,size;
			ksscanf(command,"page_map %p %p %u",&virt,&phys,&size);
			page_map(virt,phys,size,PAGE_TABLE_READWRITE);
		}
		else if(!kstrncmp(command,"page_unmap"))
		{
			u32 virt,size;
			ksscanf(command,"page_unmap %p %u",&virt,&size);
			page_unmap(virt,size);
		}
		else if(!kstrncmp(command,"page_v2p"))
		{
			u32 virt;
			ksscanf(command,"page_v2p %p",&virt);
			kprintf("VIRT %p -> PHYS %p\n",virt,page_v2p(virt));
		}
		else */
		if(!kstrncmp(command,"ram_map"))
		{
			kprintf("RAM size = %u\nRAM pools %u\n",
				lmem->ram_size,lmem->ram_pools);
			for(u32 i = 0; i < lmem->ram_pools; i++)
			{
				kprintf("\t%p\t%p\n",lmem->ram_pool[i].start,
					lmem->ram_pool[i].end);
			}
		}
		else if(!kstrncmp(command,"dump"))
		{
			u64 lines,virt;
			ksscanf(command,"dump %u %p",&lines,&virt);
			for(u64 i = 0; i < lines; i++)
			{
				kprintf("%p: ",virt);
				for(u64 j = 0; j < 16; j++)
				{
					u8 byte;
					byte = *(u8*)(virt+j);
					kprintf("%c%c ",xnums[byte>>4],xnums[byte&0x0F]);
				}
				kprintf("\n");
				virt += 16;
			}
		}
		else if(!kstrncmp(command,"pci_dump"))
		{
			pci_dump();
		}
		else if(!kstrncmp(command,"hang"))
		{
			while(1){__asm("nop");}
		}
		else if(!kstrncmp(command,"lvalloc"))
		{
			u64 size;
			ksscanf(command,"lvalloc %u",&size);
			kprintf(":\t%p\n",lvalloc(size));
		}
		else if(!kstrncmp(command,"memcpy"))
		{
			u64 dst,src,size;
			ksscanf(command,"memecpy %p %p %u",&dst,&src,&size);
			kmemcpy((void*)dst,(const void*)src,size);
		}
		else if(!kstrncmp(command,"vcopy"))
		{
			char* src;
			u64 size;
			ksscanf(command,"vcopy %p %u",&src,&size);
			for(u64 i = 0; i < size; i++) kputc(src[i]);
		}
		else if(!kstrncmp(command,"disk_read"))
		{
			char name[2];
			u64 lba,num;
			disk_t* disk;
			
			ksscanf(command,"disk_read \"%s\" %u %u",name,2,&lba,&num);
			disk = disk_get(name[0]);
			disk_read(disk,lba,num,buf);
			kprintf(":\tbuf %p\n",buf);
		}
		else if(!kstrncmp(command,"disk_info"))
		{
			kprintf("UID: %x %x\n",*(u64*)&disk1->uid,
				*((u64*)(&disk1->uid)+1));
			for(u64 i = 0; i < 4; i++)
			{
				kprintf("%x\t%x\n",disk1->parts[i].lba_start,
					disk1->parts[i].lba_num);
				kprintf("%x\t%x\n\n",disk1->parts[i].attributes,
					disk1->parts[i].type);
			}		
		}
		else if(!kstrncmp(command,"malloc"))
		{
			u64 size;
			ksscanf(command,"malloc %u",&size);
			kprintf(":\t%p\n",kmalloc(size));
		}
		else if(!kstrncmp(command,"free"))
		{
			u64 addr;
			ksscanf(command,"free %p",&addr);
			kfree((void*)addr);
		}
		else if(!kstrncmp(command,"file_load"))
		{
			u32 size;
			kmemzero(name,sizeof(name));
			ksscanf(command,"load_file \"%s\"",
				name,sizeof(name));
			void* addr = file_load(&g_fat,&size,name);
			kprintf(":\t%p\t%u\n",addr,size);
		}
		else if(!kstrncmp(command,"тестовая_команда"))
		{
			kprintf("Работает :D\n");
		}
		else if(!kstrncmp(command,"draw"))
		{
			ksscanf(command,"draw \"%s\"",name,sizeof(name));
			u8* bmp = file_load(&g_fat,NULL,name);
			if(!bmp)
			{
				kprintf("Ошибка!\n");
				continue;
			}
			printbmp(bmp);
			kfree(bmp);
		}
		/*else if(!kstrncmp(command,"sata_identify"))
		{
			sata_identify();
		}*/
	}
}


void kmain(lmem_t* lptr)
{
	lmem = lptr;
	
	cls();
	chg_color(VGA_COLOR_LIGHT_MAGENTA);
	
	idt_install();
	idt_enable_interrupts();
	
	pic_init(0x20,0x28);
	pit_init();
	
	install_exceptions();
	keyboard_install();
	__asm("sti");
	
	mbt_scan_memory();
	lvalloc_init();
	pci_detect_hardware();
	
	disk_init();
	disk1 = disk_get('D');
	disk_setup(disk1);
	
	kprintf("disk1 size %f GB (%u MB)\n",
		(((disk1->phys_totalsectors<<9)>>20)<<16)>>10,
		(disk1->phys_totalsectors<<9)>>20);
	kprintf("sizeof(lmem_t) %u\n",sizeof(lmem_t));
	
	if(lmem->ram_size < (1073741824ULL))
	{
		//We're running under QEMU
		disk1->cur_part = 1;
	}
	
	fat_init(&g_fat,disk1);
	
	//Initialize heap. Since heap was intitialized, we can't use
	//linear allocator (lvalloc) anymore
	heap_init();
	
	video_init();
	vga_terminal_init();
	
	u8* logo = file_load(&g_fat,NULL,"/logo.bmp");
	bmp_draw((lmem->v_width>>1)-80,
		0,160,lmem->vt_charh*10,logo);
	kfree(logo);
	
	for(u64 i = 0; i < 10; i++) kputc('\n');
	kprintf("Леночка x86-64\n");
	
	command_interpreter();
	
	while(1){__asm("hlt");} //Should never return!
}
