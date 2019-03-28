#ifndef __ATA_PIO_H
#define __ATA_PIO_H

#include "os.h"

#define ATA_MASTER_DRIVE 0xA0
#define ATA_SLAVE_DRIVE 0xB0

#define ATA_ERR_AMNF 	(1<<0)
#define ATA_ERR_TKZNF 	(1<<1)
#define ATA_ERR_ABRT 	(1<<2)
#define ATA_ERR_MCR 	(1<<3)
#define ATA_ERR_IDNF 	(1<<4)
#define ATA_ERR_MC 		(1<<5)
#define ATA_ERR_UNC 	(1<<6)
#define ATA_ERR_BBK 	(1<<7)

#define ATA_DRIVE_REG(lba,drv) (0x80|((drv&0x01)<<4)|((lba)&0x0F))

#define ATA_STS_ERR 	(1<<0)
#define ATA_STS_IDX 	(1<<1)
#define ATA_STS_CORR 	(1<<2)
#define ATA_STS_DRQ 	(1<<3)
#define ATA_STS_SRV 	(1<<4)
#define ATA_STS_FD 		(1<<5)
#define ATA_STS_RDY 	(1<<6)
#define ATA_STS_BSY 	(1<<7)

#define ATA_DCR_NIEN 	(1<<1)
#define ATA_DCR_SRST 	(1<<2)
#define ATA_DCR_HOB 	(1<<7)

#define ATA_PRIMARY_IO_BASE 0x1F0
#define ATA_PRIMARY_CTRL_BASE 0x3F6

#define ATA_SECONDARY_IO_BASE 0x170
#define ATA_SECONDARY_CTRL_BASE 0x376

// Port map

#define ATA_DATA 		0
#define ATA_ERR 		1
#define ATA_FEAT 		1
#define ATA_SEC 		2
#define ATA_LBALO 		3
#define ATA_LBAMID 		4
#define ATA_LBAHI 		5
#define ATA_DRV 		6
#define ATA_STATUS 		7
#define ATA_COMMAND		7

// Commands

#define ATA_CMD_IDENTIFY 0xEC

void ata_init();
void ata_init_drive(u8 drv,u8 slavebit);

#endif
