#include "bmp.h"
#include "heap.h"
#include "video.h"
#include "stdlib.h"
#include "defines.h"

void bmp_draw(u64 px,u64 py,u64 w,u64 h,u8* bmp)
{
	u32 offbits = *(u32*)(bmp+0x0A);
	u64 bw,bh,stride;
	u64 bc;
	u8* bmap = bmp+offbits;
	
	w = MIN(w,lmem->v_width);
	h = MIN(h,lmem->v_height);
	
	bw = *(u32*)(bmp+0x12);
	bh = *(u32*)(bmp+0x16);
	bc = *(u16*)(bmp+0x1C)>>3;
	
	stride = ROUND2_UP((bw*bc),2);
	
	u64 f_w,f_h;
	if(!w) f_w = 0x00010000;
	else f_w = (w<<32) / (bw<<16);
	if(!h) f_h = 0x00010000;
	else f_h = (h<<32) / (bh<<16);
	
	u64 c_y = 0;
	for(u64 y = 0; y < bh; y++)
	{
		u64 c_x = 0;
		u8* line = &bmap[(bh-y-1)*stride];
		for(u64 x = 0; x < bw; x++)
		{
			u8 r,g,b;
			r = *line++;
			g = *line++;
			b = *line++;
			u32 color = (b<<16|g<<8|r);
			for(u64 i = 0; i < (f_w>>16); i++)
			{
				video_pixel(px+c_x+i,py+c_y,color);
				for(u64 j = 1; j < (f_h>>16); j++)
					video_pixel(px+c_x+i,py+c_y+j,color);
			}
			c_x = ((c_x<<16) + f_w) >> 16;
		}
		c_y = ((c_y<<16) + f_h) >> 16;
	}
}
