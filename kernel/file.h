#ifndef __FILE_H
#define __FILE_H

#include "os.h"
#include "fat.h"

void* file_load(fat_t* fat,u32* size,const char* name);

#endif
