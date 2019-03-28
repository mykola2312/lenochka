#include "vga_terminal.h"
//#include "paging.h"
#include "video.h"
#include "heap.h"
#include "string.h"
#include "stdlib.h"
#include "defines.h"
#include "bmp.h"
#include "os.h"
#include "io.h"

static u8 s_cCurAttr = DEFAULT_COLOR;
u32 s_iX;
u32 s_iY;

static u16* s_video_buffer = 0;
static int s_video_init = 0;

static void draw_char(u16 sym,u32 color);

static u32 vga_palette[] = {
	0x000000,
	0x0000AA,
	0x00AA00,
	0x00AAAA,
	0xAA0000,
	0xAA00AA,
	0xAA5500,
	0xAAAAAA,
	
	0x555555,
	0x5555FF,
	0x55FF55,
	0x55FFFF,
	0xFF5555,
	0xFF55FF,
	0xFFFF55,
	0xFFFFFF,
};

void vga_terminal_init()
{
	s_video_buffer = (u16*)kmalloc(4000);
	s_video_init = 1;
	
	lmem->vt_chars = 80;
	lmem->vt_lines = 25;
	lmem->vt_charw = lmem->v_width/lmem->vt_chars;
	lmem->vt_charh = lmem->v_height/lmem->vt_lines;
	
	s_iX = 0;
	s_iY = 0;
}

void update_cursor()
{
	/*u16 pos;
	pos = s_iY * 80 + s_iX;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8) ((pos >> 8) & 0xFF));*/
}

void cls()
{
	if(!s_video_init) return;
	//kmemzero((void*)s_video_buffer,sizeof(u16)*80*25);
	video_clear();
	s_iX = s_iY = 0;
	update_cursor();
}

void shiftscreen();

static void draw_char(u16 sym,u32 color)
{
	if(!sym && !color)
	{
		video_rect_zero(s_iX*lmem->vt_charw,s_iY*lmem->vt_charh,
			lmem->vt_charw,lmem->vt_charh);
	}		
	video_char(s_iX*lmem->vt_charw,s_iY*lmem->vt_charh,
		lmem->vt_charw,lmem->vt_charh,color,sym);
}

void printchar(u16 c)
{	
	if(!s_video_init) return;
	if(c == '\n' || c == '\r' || c == '\t' || c == '\b') // Control characters
	{
		if(c == '\r') s_iX = 0;
		else if(c == '\n') s_iX = SCR_CHARS;
		else if(c == '\t') s_iX = (s_iX + 8) & ~7;
		else if(c == '\b')
		{
			if(s_iX) s_iX--;
			else if(s_iY) 
			{
				s_iY--;
				s_iX = SCR_CHARS-1;
			}
			//Remove character
			//SCR_TEXT_BUF[s_iY*SCR_CHARS+s_iX] = SCR_ENTRY(0,s_cCurAttr);
			draw_char(0,0);
		}
	}
	else // Text characters
	{
		//SCR_TEXT_BUF[s_iY*SCR_CHARS+s_iX++] = SCR_ENTRY(c,s_cCurAttr);
		draw_char(c,vga_palette[s_cCurAttr]);
		s_iX++;
	}
	
	if(s_iX > SCR_CHARS-1)
	{
		s_iX = 0;
		if(s_iY < SCR_LINES-1)
			s_iY++;
		else shiftscreen();
	}
	
	update_cursor();
}

void printbmp(u8* bmp)
{
	//scroll
	//u64 scroll = s_iY
	u32 h = *(u32*)(bmp+0x16);
	h = MIN(h,lmem->v_height);
	u64 lines = (h/lmem->vt_charh) + 1;
	kprintf("lines %u\n",lines);
	for(u64 i = 0; i < lines; i++) kputc('\n');
	bmp_draw(s_iX*lmem->vt_charw,(s_iY-lines-1)*lmem->vt_charh,
		0,h,bmp);
}

void shiftscreen()
{
	u32 y;
	if(!s_video_init) return;
	video_line_zero(0,lmem->vt_charh);
	for(y = 1; y < lmem->vt_lines; y++)
	{
		video_line_copy(y*lmem->vt_charh,(y-1)*lmem->vt_charh,
			lmem->vt_charh);
	}
	
	video_line_zero(lmem->vt_charh*(lmem->vt_lines-1),
		lmem->vt_charh);
}

void chg_color(u8 color)
{
	s_cCurAttr = color;
}

void print(const char* str)
{
	while(*str)
	{
		printchar(kchr2dbcs(str));
		str += kchrlen(*str);
	}
}

void move_to_next_char(){}
void move_to_next_line(){}
void locate(int h, int w)
{
	s_iX = w;
	s_iY = h;
}
