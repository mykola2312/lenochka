#ifndef __MULTIBOOT_H
#define __MULTIBOOT_H

#include "os.h"

typedef struct {
	u32 flags;
	
	u32 mem_lower;
	u32 mem_upper;
	
	u32 boot_device;
	
	u32 cmd_line;
	
	u32 mods_count;
	u32 mods_addr;
	
	u32 syms[4];
	
	u32 mmap_length;
	u32 mmap_addr;
	
	u32 drives_length;
	u32 drives_addr;
	
	u32 config_table;
	
	u32 boot_loader_name;
	
	u32 apm_table;
	
	u32 vbe_control_info;
	u32 vbe_mode_info;
	u32 vbe_mode;
	u32 vbe_interface_seg;
	u32 vbe_interface_off;
	u32 vbe_interface_len;
	
	u32 framebuffer_addr;
	u32 framebuffer_pitch;
	u32 framebuffer_width;
	u32 framebuffer_height;
	u8 framebuffer_bpp;
	u8 color_info[5];
	
} __attribute__((packed)) multiboot_hdr_t;

#define MBT_MMAP_TYPE_RAM 1

typedef struct {
	u32 size;
	u64 base_addr;
	u64 length;
	u32 type;
} __packed multiboot_mmap_t;

typedef struct {
	u32 mod_start;
	u32 mod_end;
	
	u32 cmd_line;
	u32 pad;
} __packed multiboot_module_t;

#endif
