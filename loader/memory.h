#ifndef __MEMORY_H
#define __MEMORY_H

#include "os.h"
#include "multiboot.h"

#define MAX_RAM_POOLS 32

typedef struct {
	u32 __phys start;
	u32 __phys end;
} ram_pool_t;

typedef struct {
	u32 ram_total_size;
	u32 ram_pool_count;
	ram_pool_t ram_pools[MAX_RAM_POOLS];
} ram_map_t;

//It must be called before first 1 MB got unpaged
void mbt_mmap_init(multiboot_hdr_t* mbt);
ram_map_t* ram_get_map();
u32 ram_get_size();

#endif
