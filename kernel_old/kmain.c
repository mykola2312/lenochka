#include "os.h"
#include "memory.h"
#include "vga_terminal.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "pci.h"
#include "ahci.h"
#include "exceptions.h"
#include "paging.h"
#include "keyboard.h"
#include "x86utils.h"
#include "stdlib.h"
#include "string.h"

static char* xnums = "0123456789ABCDEF";

void command_interpreter()
{
	char command[64];

	while(1)
	{
		kputc('>');
		kgets(command,sizeof(command));
		if(!kstrncmp(command,"ram_map"))
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
		else if(!kstrncmp(command,"dump"))
		{
			u32 lines,virt;
			ksscanf(command,"dump %u %p",&lines,&virt);
			for(u32 i = 0; i < lines; i++)
			{
				kprintf("%p: ",virt);
				for(u32 j = 0; j < 16; j++)
				{
					u8 byte;
					byte = *(u8*)(virt+j);
					kprintf("%c%c ",xnums[byte>>8],xnums[byte&0x0F]);
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
		else if(!kstrncmp(command,"sata_identify"))
		{
			sata_identify();
		}
	}
}

void kernel_main(multiboot_hdr_t* mbt,int magic)
{
	//Unmap first 16 MB
	//Before we unmap first 16 MB, we need to know all about MBT
	mbt_mmap_init(mbt);
	page_unmap(0,1024*1024*16);
	//Map video buffer
	vga_terminal_init();
	//Fix MBT address
	
	cls();
	chg_color(VGA_COLOR_MAGENTA);
	vga_terminal_init();
	
	kprintf("GRUB loaded Lenochka!\n");
	kprintf("MBT %p magic %x\n",mbt,magic);
	kprintf("%u KB RAM\n",ram_get_size()>>10);
	
	chg_color(VGA_COLOR_WHITE);
	kprintf("\nOS_VIDEO_BUFFER %x\n\n",OS_VIDEO_BUFFER);
	chg_color(VGA_COLOR_MAGENTA);
	
	kprintf("Installing GDT\n");
	
	gdt_install();
	
	for(int i = 0; i < 10; i++) kprintf("%d ",i);
	kprintf("\nGDT Installed\n");
	
	kprintf("Init PIC\n");
	pic_init(0x20,0x28);
	
	kprintf("Installing IDT\n");
	idt_install();
	
	for(int i = 0; i < 10; i++) kprintf("%d ",i);
	kprintf("\nIDT Installed\n");
	
	install_exceptions();
	kprintf("Exceptions installed\n");
	
	pit_init();
	keyboard_install();
	idt_enable_interrupts(); //IRQs and exceptions
	
	pci_detect_hardware();
	pci_dump();
	ahci_init();
	
	kprintf("\n\n");
	
	command_interpreter();
	
	while(1){__asm("hlt");}
}
