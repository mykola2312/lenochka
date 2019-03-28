#include "video.h"
#include "heap.h"
#include "paging.h"
#include "stdlib.h"
#include "defines.h"
#include "multiboot.h"
#include "string.h"

extern u8* x64_get_font();

void video_pixel(u64 x,u64 y,u32 color)
{
	u64 off = (y*lmem->v_stride) + (x*lmem->v_bpp);
	
	if(lmem->v_bpp == 3)
	{
		u32 dat = (*(u32*)&lmem->v_mem[off]) & 0xFF000000;
		lmem->v_mem[off] = (color & 0x00FFFFFF) | dat;
	}
	else *(u32*)(lmem->v_mem+off) = color;
}

static u8* video_font_get_bitmap(u16 sym)
{
	u64 i;
	for(i = 0; i < lmem->vf_chars; i++)
		if(lmem->vf_encodings[i] == sym) break;
	if(i == lmem->vf_chars) return NULL;
	return &lmem->vf_bitmaps[i<<4];
}

void video_char(u64 x,u64 y,u64 sizew,u64 sizeh,u32 color,u16 sym)
{
	//h size is always twice
	u64 f_w = (sizew<<32) / (8ULL<<16ULL);
	u64 f_h = (sizeh<<32) / (16ULL<<16ULL);
	u8* bmap;
	
	bmap = video_font_get_bitmap(sym);
	if(!bmap)
	{
		x64_rax_n_stop(0xDEADBEEF);
		return;
	}
	u64 c_y = y;
	for(u64 i = 0; i < 16; i++)
	{
		u64 c_x = x;
		for(u64 j = 0; j < 8; j++)
		{
			if((bmap[i]>>(7-j)) & 1)
			{
				for(u64 k = 0; k < (f_w>>16); k++)
				{
					video_pixel(c_x+k,c_y,color);
					for(u64 l = 1; l < (f_h>>16); l++)
						video_pixel(c_x+k,c_y+l,color);
				}
			}
			c_x = ((c_x<<16) + f_w) >> 16;
		}
		c_y = ((c_y<<16) + f_h) >> 16;
	}
}

void video_rect_zero(u64 x,u64 y,u64 w,u64 h)
{
	u64 len = w*lmem->v_bpp;
	for(u32 i = 0; i < h; i++)
	{
		u32* line = (u32*)&lmem->v_mem[(y+i)*lmem->v_stride+(x*lmem->v_bpp)];
		kmemzero(line,len);
	}
}

void video_line_copy(u64 ysrc,u64 ydst,u64 height)
{
	kmemcpy(lmem->v_mem+(ydst*lmem->v_stride),
		lmem->v_mem+(ysrc*lmem->v_stride),
		lmem->v_stride*height);
}

void video_line_zero(u64 y,u64 height)
{
	kmemzero(lmem->v_mem+(y*lmem->v_stride),
		height*lmem->v_stride);
}

void video_clear()
{
	kmemzero(lmem->v_mem,lmem->v_size);
}

void video_swap()
{
	//kmemcpy(lmem->v_mem,lmem->v_mem,lmem->v_size);
}

void video_init()
{
	multiboot_hdr_t* hdr = (multiboot_hdr_t*)lmem->multiboot;
	
	lmem->v_stride = hdr->framebuffer_pitch;
	lmem->v_width = hdr->framebuffer_width;
	lmem->v_height = hdr->framebuffer_height;
	lmem->v_mem = (u8*)hdr->framebuffer_addr;
	lmem->v_bpp = hdr->framebuffer_bpp>>3;
	lmem->v_size = lmem->v_stride*lmem->v_height;
	
	lmem->v_r_p = *((u8*)hdr+110);
	lmem->v_r_s = *((u8*)hdr+111);
	lmem->v_g_p = *((u8*)hdr+112);
	lmem->v_g_s = *((u8*)hdr+113);
	lmem->v_b_p = *((u8*)hdr+114);
	lmem->v_b_s = *((u8*)hdr+115);
	
	u64 size = lmem->v_stride*lmem->v_height;
	for(u64 i = 0; i < (size>>12)+1; i++)
	{
		page_map((void*)((u64)lmem->v_mem+(i<<12)),
			(void*)((u64)lmem->v_mem+(i<<12)),4096);
	}
	
	u8* font = x64_get_font();
	
	lmem->vf_chars = *(u64*)font;
	lmem->vf_wchar = (8ULL<<32ULL) / (lmem->v_width<<16);
	lmem->vf_hchar = (16ULL<<32ULL) / (lmem->v_height<<16);
	
	//lmem->vf_encodings = (u16*)(font + 8);
	//lmem->vf_bitmaps = (u8*)ROUND2_UP(((u64)lmem->vf_encodings 
	//	+ (lmem->vf_chars<<1)),3);
	lmem->vf_encodings = (u16*)kmalloc(lmem->vf_chars<<1);
	lmem->vf_bitmaps = (u8*)kmalloc(lmem->vf_chars<<4);
	
	kmemcpy(lmem->vf_encodings,font+8,lmem->vf_chars<<1);
	kmemcpy(lmem->vf_bitmaps,font+8+(lmem->vf_chars<<1),lmem->vf_chars<<4);
	
	//video_char(0,0,400,228,0x00FF00FF,kchr2dbcs("й"));
}
