#include "file.h"
#include "stdlib.h"
#include "string.h"
#include "defines.h"
#include "heap.h"

//	/dir/file.txt

static int file_find(fat_t* fat,fat_dir_t* out,const char* filename)
{
	char name[512];
	const char* s = filename;
	char* d = name;
	fat_dir_t _parent;
	fat_dir_t file;
	fat_dir_t* parent;
	
	parent = NULL;
	kmemzero(&file,sizeof(file));
	kmemzero(&_parent,sizeof(_parent));
	kmemzero(name,sizeof(name));
	do {
		if(*s != '/')
			*d++ = *s;
		s++;
		if(*s == '/')
		{
			//kprintf("name %s\n",name);
			//Find directory
			if(kstrchr(s+1,'/'))
			{
				fat_find_file(fat,parent,name,&_parent);
				if(_parent.dir_fstcluslo == 0) return 0;
			}
			kmemzero(name,sizeof(name));
			d = name;
		}
		if(!kstrchr(s+1,'/')) //Check if we hit file name
		{
			fat_find_file(fat,parent,name,&file);
			if(file.dir_fstcluslo != 0)
			{
				//kprintf("file %s\n",name);
				kmemcpy(out,&file,sizeof(file));
				return 1;
			}
		}
	} while(*s);
	return 0;
}

void* file_load(fat_t* fat,u32* psize,const char* name)
{
	fat_dir_t file;
	u32 clus;
	u32 size;
	u8* buf,*ptr;
	kmemzero(&file,sizeof(file));
	if(!file_find(fat,&file,name))
	{
		kprintf("File \"%s\" not found!\n",name);
		return NULL;
	}
	
	size = file.dir_filesize;
	if(psize) *psize = size;
	buf = (u8*)kmalloc(size);
	if(!buf)
	{
		kprintf("Not enough memory for \"%s\"!\n",name);
		return NULL;
	}
	ptr = buf;
	
	clus = (file.dir_fstclushi<<16)|file.dir_fstcluslo;
	do {
		u32 csize = MIN(size,(fat->spc*fat->bps));
		fat_read_cluster(fat,clus);
		kmemcpy(ptr,fat->clusbuf,csize);
		
		ptr += csize;
		csize -= csize;
		clus = fat_table_read_entry(fat,clus);
	} while(!IS_FAT_CLUS_EOF(clus));
	return buf;
}
