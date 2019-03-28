#include "ata_pio.h"
#include "io.h"
#include "pic.h"
#include "idt.h"
#include "pci.h"
#include "exceptions.h"
#include "stdlib.h"

typedef struct {
	u8 buf[512];
	u16 io_base;
	u16 ctrl_base;
	u32 slavebit;
	u32 wait;
} ata_pio_t;

static ata_pio_t s_ata;

void ata_interrupt_master(int idx)
{
	s_ata.wait = 0;
	kprintf("interrupt happend\n");
}

void ata_wait()
{
	__asm("cli");
	s_ata.wait = 1;
	__asm("sti");
	do {
		__asm("hlt");
	} while(s_ata.wait);
}

void ata_init_drive(u8 drv,u8 slavebit)
{
	u8 poll;
	
	if(!slavebit)
	{
		s_ata.io_base = slavebit ? 0x170 : 0xC080;
		s_ata.ctrl_base = 0x3E6;
	}
	else
	{
		s_ata.io_base = slavebit ? 0x168 : 0x1E8;
		s_ata.ctrl_base = 0x366;
	}
	
	kprintf("s_ata.io_base %x\ns_ata.ctrl_base %x\n",
		s_ata.io_base,s_ata.ctrl_base);
	
	outb(s_ata.io_base + ATA_DRV,slavebit ? 0xB0 : 0xA0);
	
	outb(s_ata.io_base + ATA_LBALO,0);
	outb(s_ata.io_base + ATA_LBAMID,0);
	outb(s_ata.io_base + ATA_LBAHI,0);
	
	outb(s_ata.io_base + ATA_COMMAND,ATA_CMD_IDENTIFY);
	
	if(inb(s_ata.io_base + ATA_STATUS) == 0) return;
	// Poll status
	while((poll=inb(s_ata.io_base + ATA_STATUS)) & ATA_STS_BSY)
	{
		if(poll == 0xFF)
		{
			kprintf("ATA Drive not found!\n");
			return;
		}
		kprintf("polling 1 (%x)\n",inb(s_ata.io_base + ATA_STATUS));
	}
	if(inb(s_ata.io_base + ATA_LBAMID) | s_ata.io_base + ATA_LBAHI)
	{
		kprintf("Non-ATA\n");
		return; // Non-ATA
	}
	// Wait DRQ
	while((inb(s_ata.io_base + ATA_STATUS) & ATA_STS_DRQ) == 0)
	{
		kprintf("polling 2\n");
		if(inb(s_ata.io_base + ATA_STATUS) & ATA_STS_ERR)
		{
			kprintf("ATA_STS_ERR\n");
			return;
		}
	}
	// Read data
	_insw256(s_ata.io_base + ATA_DATA,s_ata.buf);
	
	kprintf("LBA size %u\n",*(u32*)(&s_ata.buf[0xC0]));
}

void ata_init()
{
	pci_device_t sata;
	u32 irq;
	u32 bar[6];
	
	
	//Use PCI
	sata = pci_find_device(0x01,0x06);
	if(sata == 0)
	{
		kprintf("Serial ATA not found!\n");
		return;
	}
	
	kprintf("SATA %x\n",sata);
	for(u32 i = 0; i < 6; i++)
	{
		bar[i] = pci_read_config_word_dev(sata,0x10+(i<<2));
		kprintf("bar%u %x\n",i,bar[i]);
	}
	irq = pci_read_config_word_dev(sata,0x3C)&0xFF;
	kprintf("SATA irq %u\n",irq);
	kprintf("SATA prog_if %x\n",(pci_read_config_word_dev(sata,0x08)&0x0000FF00)>>8);
	
	pic_set_mask(14,1);
	idt_setup(_int14_handler,ata_interrupt_master,
		46,IDT_TYPE_INTERRUPT);
	
}
