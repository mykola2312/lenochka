#ifndef __AHCI_H
#define __AHCI_H

#include "os.h"

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM		0x96690101	// Port multiplier

#define ATA_DEV_BUSY 	(1<<7)
#define ATA_DEV_DRQ 	(1<<3)

typedef struct {
	//u32 clb;		// 0x00, command list base address, 1K-byte aligned
	//u32 clbu;		// 0x04, command list base address upper 32 bits
	u64 clb;
	u64 fb;
	
	u32 is;			// 0x10, interrupt status
	u32 ie;			// 0x14, interrupt enable
	u32 cmd;		// 0x18, command and status
	u32 rsv0;		// 0x1C, Reserved
	u32 tfd;		// 0x20, task file data
	u32 sig;		// 0x24, signature
	u32 ssts;		// 0x28, SATA status (SCR0:SStatus)
	u32 sctl;		// 0x2C, SATA control (SCR2:SControl)
	u32 serr;		// 0x30, SATA error (SCR1:SError)
	u32 sact;		// 0x34, SATA active (SCR3:SActive)
	u32 ci;			// 0x38, command issue
	u32 sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	u32 fbs;		// 0x40, FIS-based switch control
	u32 rsv1[11];	// 0x44 ~ 0x6F, Reserved
	u32 vendor[4];	// 0x70 ~ 0x7F, vendor specific
} hba_port_t;

typedef struct
{
	// 0x00 - 0x2B, Generic Host Control
	u32 cap;		// 0x00, Host capability
	u32 ghc;		// 0x04, Global host control
	u32 is;			// 0x08, Interrupt status
	u32 pi;			// 0x0C, Port implemented
	u32 vs;			// 0x10, Version
	u32 ccc_ctl;	// 0x14, Command completion coalescing control
	u32 ccc_pts;	// 0x18, Command completion coalescing ports
	u32 em_loc;		// 0x1C, Enclosure management location
	u32 em_ctl;		// 0x20, Enclosure management control
	u32 cap2;		// 0x24, Host capabilities extended
	u32 bohc;		// 0x28, BIOS/OS handoff control and status
 
	// 0x2C - 0x9F, Reserved
	u8 rsv[116];
 
	// 0xA0 - 0xFF, Vendor specific registers
	u8 vendor[96];
 
	// 0x100 - 0x10FF, Port control registers
	hba_port_t ports[32];
} hba_mem_t;

typedef enum
{
	FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
} fis_type_t;

typedef struct
{
	// DWORD 0
	u8 fis_type;	// FIS_TYPE_REG_H2D
 
	u8 pmport:4;	// Port multiplier
	u8 rsv0:3;		// Reserved
	u8 c:1;		// 1: Command, 0: Control
 
	u8 command;	// Command register
	u8 featurel;	// Feature register, 7:0
 
	// DWORD 1
	u8 lba0;		// LBA low register, 7:0
	u8 lba1;		// LBA mid register, 15:8
	u8 lba2;		// LBA high register, 23:16
	u8 device;		// Device register
 
	// DWORD 2
	u8 lba3;		// LBA register, 31:24
	u8 lba4;		// LBA register, 39:32
	u8 lba5;		// LBA register, 47:40
	u8 featureh;	// Feature register, 15:8
 
	// DWORD 3
	u8 countl;		// Count register, 7:0
	u8 counth;		// Count register, 15:8
	u8 icc;		// Isochronous command completion
	u8 control;	// Control register
 
	// DWORD 4
	u8 rsv1[4];	// Reserved
} fis_reg_h2d_t;

typedef struct
{
	// DWORD 0
	u8 fis_type;	// FIS_TYPE_REG_D2H
 
	u8 pmport:4;	// Port multiplier
	u8 rsv0:2;	 // Reserved
	u8 i:1;		 // Interrupt bit
	u8 rsv1:1;	 // Reserved
 
	u8 status;	 // Status register
	u8 error;	  // Error register
 
	// DWORD 1
	u8 lba0;		// LBA low register, 7:0
	u8 lba1;		// LBA mid register, 15:8
	u8 lba2;		// LBA high register, 23:16
	u8 device;	 // Device register
 
	// DWORD 2
	u8 lba3;		// LBA register, 31:24
	u8 lba4;		// LBA register, 39:32
	u8 lba5;		// LBA register, 47:40
	u8 rsv2;		// Reserved
 
	// DWORD 3
	u8 countl;	 // Count register, 7:0
	u8 counth;	 // Count register, 15:8
	u8 rsv3[2];	 // Reserved
 
	// DWORD 4
	u8 rsv4[4];	 // Reserved
} fis_reg_d2h_t;

typedef struct
{
	// DWORD 0
	u8 fis_type;	// FIS_TYPE_DATA
 
	u8 pmport:4;	// Port multiplier
	u8 rsv0:4;		// Reserved
 
	u8 rsv1[2];	// Reserved
 
	// DWORD 1 ~ N
	u32 data[1];	// Payload
} fis_data_t;

typedef struct
{
	// DWORD 0
	u8 fis_type;	// FIS_TYPE_PIO_SETUP
 
	u8 pmport:4;	// Port multiplier
	u8 rsv0:1;		// Reserved
	u8 d:1;		// Data transfer direction, 1 - device to host
	u8 i:1;		// Interrupt bit
	u8 rsv1:1;
 
	u8 status;		// Status register
	u8 error;		// Error register
 
	// DWORD 1
	u8 lba0;		// LBA low register, 7:0
	u8 lba1;		// LBA mid register, 15:8
	u8 lba2;		// LBA high register, 23:16
	u8 device;		// Device register
 
	// DWORD 2
	u8 lba3;		// LBA register, 31:24
	u8 lba4;		// LBA register, 39:32
	u8 lba5;		// LBA register, 47:40
	u8 rsv2;		// Reserved
 
	// DWORD 3
	u8 countl;		// Count register, 7:0
	u8 counth;		// Count register, 15:8
	u8 rsv3;		// Reserved
	u8 e_status;	// New value of status register
 
	// DWORD 4
	u16 tc;		// Transfer count
	u8 rsv4[2];	// Reserved
} fis_pio_setup_t;

typedef struct
{
	// DWORD 0
	u8 fis_type;	// FIS_TYPE_DMA_SETUP
 
	u8 pmport:4;	// Port multiplier
	u8 rsv0:1;		// Reserved
	u8 d:1;		// Data transfer direction, 1 - device to host
	u8 i:1;		// Interrupt bit
	u8 a:1;			// Auto-activate. Specifies if DMA Activate FIS is needed
 
	u8 rsved[2];	  // Reserved
 
	//DWORD 1&2
 
	u64 DMAbufferID;	// DMA Buffer Identifier. Used to Identify DMA buffer in host memory. SATA Spec says host specific and not in Spec. Trying AHCI spec might work.
 
	//DWORD 3
	u32 rsvd;		  //More reserved
 
	//DWORD 4
	u32 DMAbufOffset;  //Byte offset into buffer. First 2 bits must be 0
 
	//DWORD 5
	u32 TransferCount; //Number of bytes to transfer. Bit 0 must be 0
 
	//DWORD 6
	u32 resvd;		 //Reserved
 
} fis_dma_setup_t;

typedef struct
{
	// 0x00
	fis_dma_setup_t	dsfis;		// DMA Setup FIS
	u8	 pad0[4];
 
	// 0x20
	fis_pio_setup_t	psfis;		// PIO Setup FIS
	u8	 pad1[12];
 
	// 0x40
	fis_reg_d2h_t	rfis;		// Register – Device to Host FIS
	u8	 pad2[4];
 
	// 0x58
	u64	sdbfis;		// Set Device Bit FIS
 
	// 0x60
	u8	 ufis[64];
 
	// 0xA0
	u8  	rsv[0x100-0xA0];
} hba_fis_t;

typedef struct
{
	// DW0
	u8 cfl:5;		// Command FIS length in DWORDS, 2 ~ 16
	u8 a:1;		// ATAPI
	u8 w:1;		// Write, 1: H2D, 0: D2H
	u8 p:1;		// Prefetchable
 
	u8 r:1;		// Reset
	u8 b:1;		// BIST
	u8 c:1;		// Clear busy upon R_OK
	u8 rsv0:1;		// Reserved
	u8 pmp:4;		// Port multiplier port
 
	u16 prdtl;		// Physical region descriptor table length in entries
 
	// DW1
	u32 prdbc;		// Physical region descriptor byte count transferred
 
	// DW2, 3
	//u32 ctba;		// Command table descriptor base address
	//u32 ctbau;		// Command table descriptor base address upper 32 bits
	u64 ctba;
 
	// DW4 - 7
	u32 rsv1[4];	// Reserved
} hba_cmd_header_t;

typedef struct
{
	//u32 dba;		// Data base address
	//u32 dbau;		// Data base address upper 32 bits
	u64 dba;
	u32 rsv0;		// Reserved
 
	// DW3
	u32 dbc:22;		// Byte count, 4M max
	u32 rsv1:9;		// Reserved
	u32 i:1;		// Interrupt on completion
} hba_prdt_entry_t;

typedef struct
{
	// 0x00
	u8 cfis[64];	// Command FIS
 
	// 0x40
	u8 acmd[16];	// ATAPI command, 12 or 16 bytes
 
	// 0x50
	u8 rsv[48];	// Reserved
	
	// 0x80
	hba_prdt_entry_t	prdt_entry[1];	// Physical region descriptor table entries, 0 ~ 65535
} hba_cmd_tbl_t;

u64 ahci_read_sectors(hba_port_t* port,u64 lba,u64 sectors,
	u64 __phys buf);

u64 ahci_identify(hba_port_t* port);

hba_port_t* ahci_get_sata();
hba_port_t* ahci_get_atapi();

void ahci_init();

#define AHCI_CLB_SIZE ((sizeof(hba_cmd_header_t)*32)+4096)
#define AHCI_FB_SIZE ((sizeof(hba_fis_t)*32)+4096)

#define SCMD_ST (1<<0)
#define SCMD_CR (1<<15)
#define SCMD_FR (1<<14)
#define SCMD_FRE (1<<4)

#define ATA_LBA_MODE 0x40
#define ATA_CMD_IDENTIFY 0xEC
#define ATA_CMD_READ_DMA_EX 0x25

#endif
