#include "disk.h"
#include "string.h"
#include "paging.h"
#include "stdlib.h"
#include "lvalloc.h"
#include "pit.h"

typedef struct {
	u32 disk_available;
	disk_t disk[26];
} diskhost_t;

static diskhost_t s_diskhost;
static __align(4096) u8 mbr[512];

static u32 disk_chr2idx(char name)
{
	return name-'A';
}

disk_t* disk_get(char name)
{
	u32 idx = disk_chr2idx(name);
	if(idx >= 26) return NULL;
	return &s_diskhost.disk[idx];
}

u32 disk_available()
{
	u32 disks;
	for(u64 i = 0; i < 26; i++)
	{
		if(s_diskhost.disk[i].phys.type != DISK_INVALID)
			disks |= (1<<i);
	}
	return disks;
}

u64 disk_read_ex(disk_t* disk,u64 lba,u64 sectors,void* __virt buf)
{
	u64 ret,attempts = 0;
	switch(disk->phys.type)
	{
		case DISK_INVALID: break;
		case DISK_AHCI:
			do {
				ret = ahci_read_sectors(disk->phys.hba_port,lba,sectors,
					(u64)page_v2p((void*)buf));
				if(ret) sleep(1000);
			} while(ret != 0 || attempts++ < 2);
		break;
	}
	return (attempts == 2);
}

u64 disk_write_ex(disk_t* disk,u64 lba,u64 sectors,void* __virt buf)
{
	u64 ret,attempts = 0;
	switch(disk->phys.type)
	{
		case DISK_INVALID: break;
		case DISK_AHCI:
			do {
				ret = ahci_read_sectors(disk->phys.hba_port,lba,sectors,
					(u64)page_v2p((void*)buf));
				if(ret) sleep(1000);
			} while(ret != 0 || attempts++ < 2);
		break;
	}
	return (attempts == 2);
}

u64 disk_read(disk_t* disk,u64 lba,u64 sectors,void* __virt buf)
{
	kmemzero(buf,sectors<<9);
	if((lba+sectors) >= disk->parts[disk->cur_part].lba_num)
	{
		kprintf("disk_read out of bounds!\n");
		return 1;
	}
	
	return disk_read_ex(disk,lba+(u64)disk->parts[disk->cur_part].lba_start,
		sectors,buf);
}

u64 disk_write(disk_t* disk,u64 lba,u64 sectors,void* __virt buf)
{
	if((lba+sectors) >= disk->parts[disk->cur_part].lba_num)
	{
		kprintf("disk_write out of bounds!\n");
		return 1;
	}
	
	return disk_write_ex(disk,lba+(u64)disk->parts[disk->cur_part].lba_start,
		sectors,buf);
}

void disk_setup(disk_t* disk)
{
	disk->phys_totalsectors = ahci_identify(disk->phys.hba_port);
	disk_read_ex(disk,0,1,mbr);
	
	kmemcpy(&disk->uid,&mbr[0x1B4],10);
	kmemcpy(disk->parts,&mbr[0x1BE],64);
	
	//Determine which partition we should use
	disk->cur_part = 0;
	do {
		disk_read(disk,0,1,mbr);
		if(!kmemcmp((char*)&mbr[71],"DISK1",5))
		{
			//That's FAT32
			break;
		}
		disk->cur_part = disk->cur_part + 1;
	} while(disk->cur_part < 4);
}

void disk_init()
{
	disk_t* disk;
	
	//s_diskhost.mbr = (u8*)lvalloc(4096);
	//kmemzero(&s_diskhost,sizeof(s_diskhost));
	ahci_init();
	
	//SATA (D)
	disk = &s_diskhost.disk[disk_chr2idx('D')];
	disk->phys.type = DISK_AHCI;
	disk->phys.hba_port = ahci_get_sata();
	//get sectors
	
	//ATAPI (E)
	disk = &s_diskhost.disk[disk_chr2idx('E')];
	disk->phys.type = DISK_AHCI;
	disk->phys.hba_port = ahci_get_atapi();
}
