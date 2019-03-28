#include "ahci.h"
#include "pci.h"
#include "pit.h"
#include "io.h"
#include "lvalloc.h"
#include "paging.h"
#include "stdlib.h"

typedef struct {
	u64 hba_phys;
	hba_port_t* sata;
	hba_port_t* atapi;
	u16* buf;
} ahci_t;

static ahci_t s_ahci;
static hba_mem_t* HBA;

void ahci_issue_cmd(hba_port_t* port,u64 cmd)
{
	if(!(port->cmd & SCMD_ST))
		port->cmd |= SCMD_ST;
	port->cmd |= (1<<28);
		
	port->serr = port->serr;
	port->is = port->is;
	
	//If idle, reset PxCMD
	/*if(port->cmd & (0x7F<<23))
	{
		port->cmd &= ~SCMD_ST;
		
		port->cmd |= SCMD_ST|SCMD_FRE|(1<<3);
	}*/
	
	port->ci = (1<<cmd);
	
	//Wait for command
	while(port->tfd & ATA_DEV_BUSY)
	{
	}
}

u64 ahci_read_sectors(hba_port_t* port,u64 lba,u64 sectors,
	u64 __phys buf)
{
	hba_cmd_header_t* cmdhdr = (hba_cmd_header_t*)port->clb;
	hba_cmd_tbl_t* cmdtbl = (hba_cmd_tbl_t*)cmdhdr->ctba;
	fis_reg_h2d_t* fis = (fis_reg_h2d_t*)&cmdtbl->cfis;
	
	port->is = 0xFFFFFFFF;
	
	cmdhdr->cfl = sizeof(fis_reg_h2d_t)/sizeof(u32);
	cmdhdr->w = 0;
	cmdhdr->prdtl = (sectors>>4)+1;
	
	kmemzero(cmdtbl, sizeof(hba_cmd_tbl_t) +
 		(cmdhdr->prdtl-1)*sizeof(hba_prdt_entry_t));
	kmemzero(fis,sizeof(fis_reg_h2d_t));
	
	fis->fis_type = FIS_TYPE_REG_H2D;
	fis->c = 1;
	fis->command = ATA_CMD_READ_DMA_EX;
	
	fis->lba0 = lba&0xFF;
	fis->lba1 = (lba>>8)&0xFF;
	fis->lba2 = (lba>>16)&0xFF;
	fis->device = ATA_LBA_MODE;
	fis->lba3 = (lba>>24)&0xFF;
	fis->lba4 = (lba>>32)&0xFF;
	fis->lba5 = (lba>>36)&0xFF;
	
	fis->countl = sectors&0xFF;
	fis->counth = (sectors>>8)&0xFF;
	
	u64 i;
	for(i = 0; i < (u64)(cmdhdr->prdtl-1); i++)
	{
		kmemzero(&cmdtbl->prdt_entry[i],sizeof(hba_prdt_entry_t));
		cmdtbl->prdt_entry[i].dba = buf;
		cmdtbl->prdt_entry[i].dbc = 8191;
		cmdtbl->prdt_entry[i].i = 1;
		
		buf += 8192;
		sectors -= 16;
	}
	cmdtbl->prdt_entry[i].dba = buf;
	cmdtbl->prdt_entry[i].dbc = (sectors<<9)-1;
	cmdtbl->prdt_entry[i].i = 1;
	
	ahci_issue_cmd(port,0);
	if(port->is & (1<<30))
	{
		kprintf("disk read error!\n");
		return 1;
	}
	return 0;
}

u64 ahci_identify(hba_port_t* port)
{
	hba_cmd_header_t* cmdhdr = (hba_cmd_header_t*)port->clb;
	hba_cmd_tbl_t* cmdtbl = (hba_cmd_tbl_t*)cmdhdr->ctba;
	fis_reg_h2d_t* fis = (fis_reg_h2d_t*)&cmdtbl->cfis;
	
	port->is = 0xFFFFFFFF;
	
	cmdhdr->cfl = sizeof(fis_reg_h2d_t)/sizeof(u32);
	cmdhdr->w = 0;
	cmdhdr->prdtl = 1;
	
	kmemzero(cmdtbl, sizeof(hba_cmd_tbl_t) +
 		(cmdhdr->prdtl-1)*sizeof(hba_prdt_entry_t));
	kmemzero(fis,sizeof(fis_reg_h2d_t));
	
	fis->fis_type = FIS_TYPE_REG_H2D;
	fis->c = 1;
	fis->command = ATA_CMD_IDENTIFY;
	
	fis->lba0 = 0;
	fis->lba1 = 0;
	fis->lba2 = 0;
	fis->device = ATA_LBA_MODE;
	fis->lba3 = 0;
	fis->lba4 = 0;
	fis->lba5 = 0;
	
	fis->countl = 1;
	fis->counth = 0;
	
	cmdtbl->prdt_entry[0].dba = (u64)s_ahci.buf;
	cmdtbl->prdt_entry[0].dbc = 511;
	cmdtbl->prdt_entry[0].i = 1;
	
	ahci_issue_cmd(port,0);
	
	return *(u64*)(&s_ahci.buf[100]);
}

void port_start(hba_port_t* port)
{
	//while(port->cmd & (1<<15)){__asm("nop");}
	
	port->cmd |= SCMD_ST;
	port->cmd |= SCMD_FRE;
}

void port_stop(hba_port_t* port)
{
	if(!(port->cmd & SCMD_ST)) return;
	
	port->cmd &= ~SCMD_ST;
	
	port->cmd &= ~SCMD_ST;
	while(port->cmd & SCMD_CR)
	{
		sleep(1000);
	}
			
	port->cmd &= ~SCMD_FRE;
	while(port->cmd & SCMD_FR)
	{
		sleep(1000);
	}
	
	port->cmd &= ~SCMD_FRE;
}

void port_rebase(hba_port_t* port)
{
	port_stop(port);
	
	
	port->clb = (u64)lvalloc(4096);
	port->fb = (u64)lvalloc(4096);
	
	for(u64 i = 0; i < 32; i++)
	{
		hba_cmd_header_t* cmdhdr = &((hba_cmd_header_t*)port->clb)[i];
		
		cmdhdr->prdtl = 8;
		cmdhdr->ctba = (u64)lvalloc(4096);
	}
	
	port_start(port);
}

hba_port_t* ahci_get_sata()
{
	return s_ahci.sata;
}

hba_port_t* ahci_get_atapi()
{
	return s_ahci.atapi;
}

void ahci_init()
{
	pci_device_t ahci;
	u32 bar;
	
	//Use PCI
	ahci = pci_find_device(0x01,0x06);
	if(ahci == 0)
	{
		kprintf("AHCI not found!\n");
		return;
	}
	bar = pci_read_config_word_dev(ahci,0x24);
	s_ahci.hba_phys = (u64)(bar & 0xFFFFFFF0);
	
	kprintf("HBA %x\n",s_ahci.hba_phys);
	//Identity map
	page_map((void*)s_ahci.hba_phys,(void*)s_ahci.hba_phys,
		16384);
	HBA = (hba_mem_t*)s_ahci.hba_phys;
	
	HBA->ghc |= (1<<31); //Enable GHC.AE
	if(HBA->cap & (1<<27))
	{
		kprintf("AHCI supports stagered spin-up\n");
	}
	if(HBA->cap & (1<<31))
	{
		kprintf("AHCI supports 64-bit mode\n");
	}
	
	s_ahci.sata = NULL;
	s_ahci.atapi = NULL;
	//kprintf("pi %x\n",HBA->pi);
	for(u32 i = 0; i < 32; i++)
	{
		hba_port_t* port = &HBA->ports[i];
		if(!((HBA->pi>>i)&1)) continue;
		switch(port->sig)
		{
			case SATA_SIG_ATA: s_ahci.sata = port; break;
			case SATA_SIG_ATAPI: s_ahci.atapi = port; break;
		}
		
		if(port->cmd & (SCMD_ST|SCMD_FR|SCMD_CR|SCMD_FRE)) //Isn't IDLE
		{
			port->cmd &= ~SCMD_ST;
			while(port->cmd & SCMD_CR)
			{
				sleep(1000);
			}
			
			port->cmd &= ~SCMD_FRE;
			while(port->cmd & SCMD_FR)
			{
				sleep(1000);
			}
		}
		
		//spin-up
		port->cmd |= (1<<1);
		
		//Start it again
		//port->cmd |= SCMD_ST|SCMD_FRE|(1<<2);
		
		//Initialize port
		port_rebase(port);
		
		port->is = 0xFFFFFFFF; //RWC
		//port->is = 0xFD8000AF;
		
		//power on
		port->cmd |= (1<<2);
	}
	
	s_ahci.buf = (u16*)lvalloc(4096);
	
	ahci_identify(s_ahci.sata);
}
