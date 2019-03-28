#ifndef __OS_H
#define __OS_H

#define OS_NULL_SELECTOR 0x00
#define OS_CODE_SELECTOR 0x08
#define OS_DATA_SELECTOR 0x10

#define OS_PM86_MEM_SIZE_4K (1024*1024)

//#define OS_VIDEO_BUFFER 0xC10B8000
#define OS_VIDEO_BUFFER 0x00000000000B8000

#define __packed __attribute__((packed))
#define __align(N) __attribute__((aligned(N)))
#define __format_arg(N) __attribute__((format_arg(N)))

#define __phys
#define __virt

#define NULL (void*)0

#define KERNEL_SIZE 0x1000000

#define MAX_RAM_POOLS 8
#define MAX_LV_SSIZE 8

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef char i8;
typedef short i16;
typedef int i32;
typedef long i64;

typedef unsigned long size_t;
typedef unsigned long ulong;
typedef long ilong;

typedef struct {
	u64 __phys start;
	u64 __phys end;
} ram_pool_t;

typedef struct mblock_s {
	struct mblock_s* prev;
	struct mblock_s* next;
	u64 size;
	u32 data[1];
} mblock_t;

typedef struct {
	u64* heap;		//heap ptr
	u64 heapsz; 	//heap size in 2 MB blocks
} mheap_t;

typedef struct {
	//LOADER
	
	u64 gdt_entry[8];
	
	//Paging
	u64* p_pml4; 	//[512]
	u64* p_pdp;		//[OS_MAX_MEM_GBS]
	u64* p_dir;		//[512*OS_MAX_MEM_GBS]
	u64* p_table;	//[512*512*OS_MAX_MEM_GBS]
	
	//Loader
	u64 elf_ptr;
	u64 paging_size;
	u64 multiboot;
	u64 gdt_limit;
	
	u64 kernel_phys_base;
	u64 kernel_phys_page;
	u64 kernel_phys_addr;
	u64 kernel_virt_addr;
	
	//KERNEL
	
	//RAM
	u64 ram_size;
	u64 ram_pools;
	ram_pool_t ram_pool[MAX_RAM_POOLS];
	
	//lvalloc
	u8* lv_start;
	u64 lv_current;
	u64 lv_sp;
	u64 lv_stack[8];
	
	//heap
	mheap_t heap;
	
	//video
	u64 v_stride;
	u64 v_width;
	u64 v_height;
	u8* v_mem;
	
	u64 v_bpp;
	u64 v_size;
	
	u64 v_r_p;
	u64 v_r_s;
	
	u64 v_g_p;
	u64 v_g_s;
	
	u64 v_b_p;
	u64 v_b_s;
	
	//video text
	u64 vf_chars;
	u64 vf_wchar; //fixed point
	u64 vf_hchar; //fixed point
	
	u16* vf_encodings;
	u8* vf_bitmaps;
	
	//text
	u64 vt_charw;
	u64 vt_charh;
	u64 vt_chars;
	u64 vt_lines;
} __packed lmem_t;

extern lmem_t* lmem;

extern void x64_rax_n_stop(u64 rax);

#endif
