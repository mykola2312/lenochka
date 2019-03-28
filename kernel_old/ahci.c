#include "ahci.h"
#include "pci.h"
#include "pit.h"
#include "io.h"
#include "paging.h"
#include "stdlib.h"

typedef struct {
	u32 hba_phys;
	hba_port_t* sata;
	hba_port_t* atapi;
} ahci_t;

static ahci_t s_ahci;
static hba_mem_t* HBA;

static __align(4096) u16 buf[256];

void sata_identify()
{
	hba_cmd_header_t* cmdhdr = (hba_cmd_header_t*)s_ahci.sata->clb;
	hba_cmd_tbl_t* cmdtbl = (hba_cmd_tbl_t*)cmdhdr->ctba;
	fis_reg_h2d_t* fis = (fis_reg_h2d_t*)&cmdtbl->cfis;
	kprintf("cmdhdr %x cmdtbl %x fis %x\n",cmdhdr,cmdtbl,fis);
	s_ahci.sata->is = 0xFFFFFFFF;
	
	cmdhdr->cfl = sizeof(fis_reg_h2d_t)/sizeof(u32);
	cmdhdr->w = 0;
	cmdhdr->prdtl = 1;
	
	kmemzero(cmdtbl, sizeof(hba_cmd_tbl_t) +
 		(cmdhdr->prdtl-1)*sizeof(hba_prdt_entry_t));
	
	kmemzero(fis,sizeof(fis_reg_h2d_t));
	/*fis->fis_type = FIS_TYPE_REG_H2D;
	fis->c = 1;
	fis->command = ATA_CMD_IDENTIFY;
	fis->device = ATA_LBA_MODE; // Master*/
	
	fis->fis_type = FIS_TYPE_REG_H2D;
	fis->c = 1;
	fis->command = 0xEC;
	
	fis->lba0 = 0;
	fis->lba1 = 0;
	fis->lba2 = 0;
	fis->device = ATA_LBA_MODE;
	fis->lba3 = 0;
	fis->lba4 = 0;
	fis->lba5 = 0;
	
	fis->countl = 1;
	fis->counth = 0;
	
	kmemzero(&cmdtbl->prdt_entry[0],sizeof(hba_prdt_entry_t));
	cmdtbl->prdt_entry[0].dba = page_v2p((u32)buf);
	cmdtbl->prdt_entry[0].dbc = 511; // 512 bytes
	cmdtbl->prdt_entry[0].i = 0;
	
	kprintf("s_ahci.sata->tfd %x\n",s_ahci.sata->tfd);
	kprintf("s_ahci.sata->ci %x\n",s_ahci.sata->ci);
	kprintf("s_ahci.sata->sact %x\n",s_ahci.sata->sact);
	_debug();
	
	//Wait before executing new commands
	while(s_ahci.sata->tfd & (ATA_DEV_BUSY|ATA_DEV_DRQ))
	{
		kprintf("wait to issue cmd\n");
	}
	
	s_ahci.sata->ci = (1<<0);
	
	while((s_ahci.sata->ci & (1<<0)) != 0)
	{
		if(s_ahci.sata->is & (1<<5))
		{
			kprintf("disk error\n");
			return;
		}
		kprintf("wait\n");
	}
	
	kprintf("IDENTIFY command executed!\n");
	kprintf("%x %x %u",buf,buf[83],*(u32*)&buf[60]);
	
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
	s_ahci.hba_phys = bar & 0xFFFFFFF0;
	
	kprintf("HBA %x\n",s_ahci.hba_phys);
	//Identity map
	page_map(s_ahci.hba_phys,s_ahci.hba_phys,
		16384,PAGE_TABLE_READWRITE);
	HBA = (hba_mem_t*)s_ahci.hba_phys;
	
	HBA->ghc |= (1<<31); //Enable GHC.AE
	if(HBA->cap & (1<<27))
	{
		kprintf("AHCI supports stagered spin-up\n");
	}
	
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
		
		//Initialize port
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
		
		//Start it again
		port->cmd |= SCMD_ST|SCMD_FRE;
		
		//Clear SERR
		port->serr = 0xFFFFFFFF;
		port->ie = 0;
		
		u32 data = port->is;
		port->is = data;
	}
	
	HBA->is = 0;
	
	page_map(s_ahci.sata->clb,s_ahci.sata->clb,
		AHCI_CLB_SIZE,PAGE_DIR_READWRITE);
	page_map(s_ahci.sata->fb,s_ahci.sata->fb,
		AHCI_FB_SIZE,PAGE_DIR_READWRITE);
	
	page_map(s_ahci.atapi->clb,s_ahci.sata->clb,
		AHCI_CLB_SIZE,PAGE_DIR_READWRITE);
	page_map(s_ahci.atapi->fb,s_ahci.sata->fb,
		AHCI_FB_SIZE,PAGE_DIR_READWRITE);
	
	//Initialize SATA and ATAPI
	//sata_identify();
}
