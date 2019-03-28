#ifndef __FAT_H
#define __FAT_H

#include "os.h"
#include "disk.h"
#include "string.h"

#define FAT_BAD_CLUSTER 0xFFFFFF7
#define IS_FAT_CLUS_RESVD(a) (a >= 0xFFFFFF8 && a <= 0xFFFFFFE)
#define FAT_END_CLUSTER 0xFFFFFFF

#define FAT_CLUSTER(a) (a&0x0FFFFFFF)
#define IS_FAT_CLUS_EOF(clus) ((clus >= FAT_BAD_CLUSTER) || (clus == 0))

#define FAT_ATTR_READONLY 	0x01
#define FAT_ATTR_HIDDEN 	0x02
#define FAT_ATTR_SYSTEM 	0x04
#define FAT_ATTR_VOLUME_ID 	0x08
#define FAT_ATTR_DIRECTORY 	0x10
#define FAT_ATTR_ARCHIVE 	0x20
#define FAT_ATTR_LONG_NAME (FAT_ATTR_READONLY	\
	|FAT_ATTR_HIDDEN							\
	|FAT_ATTR_SYSTEM							\
	|FAT_ATTR_VOLUME_ID)

#define IS_VALID_FAT(fat) (fat->disk != NULL)

typedef struct {
	disk_t* disk;
	
	u32 bps;			//11	2
	u32 spc;			//13	1
	u32 rsvdsec;		//14	2
	u32 fatnum;			//16	1
	
	u32 rootentcnt;		//17	2
	u32 totsec;			//32	4
	u32 fatsz;			//36	4
	u32 rootclus;		//44	4
	
	u32 fatentperclus;
	u32 dirsperclus;
	
	u32 fatsec;
	u32 datasec;
	
	u8* secbuf;
	u8* clusbuf;
	u8* clusbuf_ex;
	u32* fatbuf;
	
	u32 cursec;
	u32 curclus;
	u32 curfatclus;
} fat_t;

typedef struct {
	char dir_name[11];
	u8 dir_attr;
	u8 dir_ntres;
	u8 dir_crttimetenth;
	u16 dir_crttime;
	u16 dir_crtdate;
	u16 dir_lstaccdate;
	u16 dir_fstclushi;
	u16 dir_wrttime;
	u16 dir_wrtdate;
	u16 dir_fstcluslo;
	u32 dir_filesize;
} __packed fat_dir_t;

#define IS_FAT_LDIR_LAST(ord) (ord&0x40)

typedef struct {
	u8 ldir_ord;
	dbcs_t ldir_name1[5];
	u8 ldir_attr;
	u8 ldir_type;
	u8 ldir_chksum;
	dbcs_t ldir_name2[6];
	u16 ldir_fstcluslo;
	dbcs_t ldir_name3[2];
} __packed fat_ldir_t;

void fat_init(fat_t* fat,disk_t* disk);

void fat_read_cluster(fat_t* fat,u32 clus);
u32 fat_table_read_entry(fat_t* fat,u32 idx);
void fat_read_file(fat_t* fat,fat_dir_t* file,u8* __virt buf);
void fat_nametofmt38(const char* name,char* out);
void fat_fmt38toname(const char* name,char* out);
int fat_is_valid_file(fat_dir_t* file);

u64 fat_first_file(fat_t* fat,fat_dir_t* parent,fat_dir_t* file,
	char* longname);
u64 fat_next_file(fat_t* fat,u64 val,fat_dir_t* file,char* longname);
int fat_find_file(fat_t* fat,fat_dir_t* parent,char* name,
	fat_dir_t* out);

extern fat_t g_fat;

#endif
