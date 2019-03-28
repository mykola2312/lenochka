#ifndef __HEAP_H
#define __HEAP_H

#include "os.h"

void heap_init();

void* kmalloc(u64 size);
void* krealloc(void* addr,u64 newsize);
void kfree(void* addr);

#endif
