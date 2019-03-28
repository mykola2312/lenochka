#include "fat.h"
#include "string.h"
#include "stdlib.h"
#include "lvalloc.h"
#include "defines.h"

fat_t g_fat;

void fat_read_cluster(fat_t* fat,u32 clus)
{
	if(!IS_VALID_FAT(fat)) return;
	
	disk_read(fat->disk,fat->datasec+((clus-2)*fat->spc),fat->spc,
		fat->clusbuf);
}

static u32 fat_dir_clus(fat_dir_t* dir)
{
	return (dir->dir_fstclushi<<16)|dir->dir_fstcluslo;
}

static u32 fat_dir_or_root_clus(fat_t* fat,fat_dir_t* dir)
{
	if(dir != NULL)
		return fat_dir_clus(dir);
	return fat->rootclus;
}

u32 fat_table_read_entry(fat_t* fat,u32 idx)
{
	//Do some caching
	u32 clus = idx / fat->fatentperclus;
	u32 offset = idx % fat->fatentperclus;
	
	//if(!IS_VALID_FAT(fat)) return 0;
	
	//kprintf("entry clus %u offset %u\n",
	//	fat->fatsec+(clus*fat->spc),offset);
	if(clus == fat->curfatclus)
	{
		disk_read(fat->disk,fat->fatsec+(clus*fat->spc),fat->spc,fat->fatbuf);
		fat->curfatclus = clus;
	}
	
	idx = FAT_CLUSTER(fat->fatbuf[offset]);
	//kprintf("fat->fatbuf[offset] %u\n",idx);
	//kprintf("%p\n",fat->fatbuf);
	return FAT_CLUSTER(fat->fatbuf[offset]);
	//kprintf("fat->fatbuf[offset] %u\n",(u64)FAT_CLUSTER(fat->fatbuf[offset]));
	//return FAT_CLUSTER(fat->fatbuf[offset]);
}

void fat_read_file(fat_t* fat,fat_dir_t* file,u8* __virt buf)
{
	if(!IS_VALID_FAT(fat)) return;
	
	u32 clus = fat_dir_clus(file);
	u32 size = file->dir_filesize;
	u32 csize = fat->bps*fat->spc;
	do {
		u32 _size = MIN(size,csize);
		
		fat_read_cluster(fat,clus);
		kmemcpy(buf,fat->clusbuf,_size);
		buf += _size;
		size -= _size;
		
		clus = fat_table_read_entry(fat,clus);
	} while(!IS_FAT_CLUS_EOF(clus));
}

void fat_nametofmt38(const char* name,char* out)
{
	char* d = out;
	const char* s = name;
	size_t slen = kstrlen(name);
	
	for(u64 i = 0; i < 11; i++)
		out[i] = ' ';
	while(*s != '.' && (size_t)(d-out) < slen)
		*d++ = ktoupper(*s++);
	if((d-out) == 11) return;
	d = &out[8];
	s++;
	for(u64 i = 0; i < 3 && *s; i++)
		*d++ = ktoupper(*s++);
}

void fat_fmt38toname(const char* name,char* out)
{
	char* d = out;
	const char* s = name;
	
	for(u64 i = 0; *s != ' ' && i < 11; i++)
		*d++ = *s++;
	if(name[8] != ' ' || name[9] != ' ' || name[10] != ' ')
	{
		*d++ = '.';
		s = &name[8];
		for(u64 i = 0; i < 3 && *s != ' '; i++)
			*d++ = *s++;
	}
	*d++ = '\0';
}

int fat_is_valid_file(fat_dir_t* file)
{
	if(file->dir_name[0] != 0 && file->dir_name[0] != (char)0xE5
				&& file->dir_attr != FAT_ATTR_LONG_NAME
				&& file->dir_attr != FAT_ATTR_VOLUME_ID)
	{
		return 1;
	}
	return 0;
}

u64 fat_first_file(fat_t* fat,fat_dir_t* parent,fat_dir_t* file,
	char* longname)
{
	char* d;
	int load_long = 1;
	
	u32 dir_clus = fat_dir_or_root_clus(fat,parent);
	do {
		fat_read_cluster(fat,dir_clus);
		for(u32 i = 0; i < fat->dirsperclus; i++)
		{
			union {
				fat_dir_t* file;
				fat_ldir_t* ldir;
			} tmp;
			tmp.file = &((fat_dir_t*)fat->clusbuf)[i];
			if(tmp.ldir->ldir_attr == FAT_ATTR_LONG_NAME && load_long)
			{
				if(IS_FAT_LDIR_LAST(tmp.ldir->ldir_ord))
				{
					tmp.ldir->ldir_ord &= ~(1<<6);
					load_long = 0;
				}
				d = longname+(13*(tmp.ldir->ldir_ord-1));
				for(u32 j = 0; j < 5; j++)
				{
					if(tmp.ldir->ldir_name1[j] == 0xFFFF)
					{
						load_long = 0;
						continue;
					}
					*d++ = DBCS2SBCS(tmp.ldir->ldir_name1[j]);
				}
				for(u32 j = 0; j < 6; j++)
				{
					if(tmp.ldir->ldir_name2[j] == 0xFFFF)
					{
						load_long = 0;
						continue;
					}
					*d++ = DBCS2SBCS(tmp.ldir->ldir_name2[j]);
				}
				for(u32 j = 0; j < 2; j++)
				{
					if(tmp.ldir->ldir_name3[j] == 0xFFFF)
					{
						load_long = 0;
						continue;
					}
					*d++ = DBCS2SBCS(tmp.ldir->ldir_name3[j]);
				}
			}
			else if(fat_is_valid_file(tmp.file))
			{
				kmemcpy(file,tmp.file,sizeof(fat_dir_t));
				return ((u64)dir_clus<<32)|(i+1);
			}
		}
		
		dir_clus = fat_table_read_entry(fat,dir_clus);
	} while(!IS_FAT_CLUS_EOF(dir_clus));
	return 0;
}

u64 fat_next_file(fat_t* fat,u64 val,fat_dir_t* file,char* longname)
{
	char* d;
	int load_long = 1;
	
	u32 dir_clus = val>>32;
	u32 offset = val&0xFFFFFFFF;
	if(offset == fat->dirsperclus)
	{
		offset = 0;
		dir_clus = fat_table_read_entry(fat,dir_clus);
	}
	
	while(!IS_FAT_CLUS_EOF(dir_clus))
	{
		u32 i = 0;
		if(offset)
		{
			i = offset;
			offset = 0;
		}
		
		fat_read_cluster(fat,dir_clus);
		for(;i < fat->dirsperclus; i++)
		{
			union {
				fat_dir_t* file;
				fat_ldir_t* ldir;
			} tmp;
			tmp.file = &((fat_dir_t*)fat->clusbuf)[i];
			if(tmp.ldir->ldir_attr == FAT_ATTR_LONG_NAME && load_long)
			{
				if(IS_FAT_LDIR_LAST(tmp.ldir->ldir_ord))
				{
					tmp.ldir->ldir_ord &= ~(1<<6);
					load_long = 0;
				}
				d = longname+(13*(tmp.ldir->ldir_ord-1));
				for(u32 j = 0; j < 5; j++)
				{
					if(tmp.ldir->ldir_name1[j] == 0xFFFF)
					{
						load_long = 0;
						continue;
					}
					*d++ = DBCS2SBCS(tmp.ldir->ldir_name1[j]);
				}
				for(u32 j = 0; j < 6; j++)
				{
					if(tmp.ldir->ldir_name2[j] == 0xFFFF)
					{
						load_long = 0;
						continue;
					}
					*d++ = DBCS2SBCS(tmp.ldir->ldir_name2[j]);
				}
				for(u32 j = 0; j < 2; j++)
				{
					if(tmp.ldir->ldir_name3[j] == 0xFFFF)
					{
						load_long = 0;
						continue;
					}
					*d++ = DBCS2SBCS(tmp.ldir->ldir_name3[j]);
				}
			}
			else if(fat_is_valid_file(tmp.file))
			{
				kmemcpy(file,tmp.file,sizeof(fat_dir_t));
				return ((u64)dir_clus<<32)|(i+1);
			}
		}
		
		dir_clus = fat_table_read_entry(fat,dir_clus);
	}
	return 0;
}

int fat_find_file(fat_t* fat,fat_dir_t* parent,char* name,
	fat_dir_t* out)
{
	u64 handle;
	fat_dir_t file;
	char longname[256];
	char name38[11];
	if(!IS_VALID_FAT(fat)) return 0;
	fat_nametofmt38(name,name38);
	
	kmemzero(longname,sizeof(longname));
	handle = fat_first_file(fat,parent,&file,longname);
	if(handle)
	{
		do {
			if(*longname != '\0')
			{
				if(!kstrcmp(longname,name))
				{
					kmemcpy(out,&file,sizeof(fat_dir_t));
					return 1;
				}
			}
			else
			{
				if(!kmemcmp(file.dir_name,name38,11))
				{
					kmemcpy(out,&file,sizeof(fat_dir_t));
					return 1;
				}
			}
			kmemzero(longname,sizeof(longname));
		} while((handle = fat_next_file(fat,handle,&file,longname)));
	}
	
	return 0;
}

void fat_init(fat_t* fat,disk_t* disk)
{
	fat->disk = disk;
	fat->secbuf = (u8*)lvalloc(4096);
	
	if(disk_read(fat->disk,0,1,fat->secbuf))
	{
		kprintf("Failed to init fat!\n");
		fat->disk = NULL;
		return;
	}
	
	fat->bps = *(u16*)(&fat->secbuf[11]);
	fat->spc = *(u8*)(&fat->secbuf[13]);
	fat->rsvdsec = *(u16*)(&fat->secbuf[14]);
	fat->fatnum = *(u8*)(&fat->secbuf[16]);
	
	fat->rootentcnt = *(u16*)(&fat->secbuf[17]);
	fat->totsec = *(u32*)(&fat->secbuf[32]);
	fat->fatsz = *(u32*)(&fat->secbuf[36]);
	fat->rootclus = *(u32*)(&fat->secbuf[44]);
	
	fat->clusbuf = (u8*)lvalloc(fat->spc*fat->bps);
	fat->fatbuf = (u32*)lvalloc(fat->spc*fat->bps);
	
	fat->fatsec = fat->rsvdsec;
	fat->datasec = fat->fatsec+(fat->fatsz*fat->fatnum);
	
	fat->fatentperclus = (fat->bps>>2)*fat->spc;
	fat->dirsperclus = (fat->bps>>5)*fat->spc;
	
	fat->cursec = 0;
	fat->curclus = 0;
	fat->curfatclus = 0;
}
