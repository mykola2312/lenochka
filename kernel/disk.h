#ifndef __DISK_H
#define __DISK_H

#include "os.h"
#include "ahci.h"

typedef enum {
	DISK_INVALID = 0,
	DISK_AHCI = 1,
} disk_type_t;

struct disk_phys_t {
	disk_type_t type;
	union {
		hba_port_t* hba_port;
	};
};

typedef struct {
	u8 attributes;
	u8 chs_start[3];
	
	u8 type;
	u8 chs_last[3];
	
	u32 lba_start;
	u32 lba_num;
} __packed partition_t;

typedef struct {
	struct disk_phys_t phys;
	char disk_name; //ASCII char A-Z
	u64 phys_totalsectors;
	u8 uid[10];
	partition_t parts[4];
	u64 cur_part;
} disk_t;

disk_t* disk_get(char name);
u32 disk_available(); //return bitmap A-Z (26 bits)
u64 disk_read_ex(disk_t* disk,u64 lba,u64 sectors,void* __virt buf);
u64 disk_write_ex(disk_t* disk,u64 lba,u64 sectors,void* __virt buf);

u64 disk_read(disk_t* disk,u64 lba,u64 sectors,void* __virt buf);
u64 disk_write(disk_t* disk,u64 lba,u64 sectors,void* __virt buf);

void disk_setup(disk_t* disk);
void disk_init();

#endif
