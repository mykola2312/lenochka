#include "vga_terminal.h"
#include "os.h"
#include "io.h"

static u8 s_cCurAttr = DEFAULT_COLOR;
u32 s_iX;
u32 s_iY;

void vga_terminal_init()
{
	//Map 0xB8000 to 0xC100B8000
	//page_map(OS_VIDEO_BUFFER,0x000B8000,80*25,PAGE_TABLE_READWRITE);
	
}

void update_cursor()
{
	u16 pos;
	pos = s_iY * 80 + s_iX;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8) ((pos >> 8) & 0xFF));
}

void cls()
{
	u32 i;
	for(i = 0; i < SCR_CHARS*SCR_LINES; i++)
		SCR_TEXT_BUF[i] = SCR_ENTRY(0,DEFAULT_COLOR);
	s_iX = s_iY = 0;
	update_cursor();
}

void shiftscreen();

void printchar(unsigned char c)
{	
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
			SCR_TEXT_BUF[s_iY*SCR_CHARS+s_iX] = SCR_ENTRY(0,s_cCurAttr);
		}
	}
	else // Text characters
		SCR_TEXT_BUF[s_iY*SCR_CHARS+s_iX++] = SCR_ENTRY(c,s_cCurAttr);
	
	if(s_iX > SCR_CHARS-1)
	{
		s_iX = 0;
		if(s_iY < SCR_LINES-1)
			s_iY++;
		else shiftscreen();
	}
	
	update_cursor();
}

void shiftscreen()
{
	//cls();
	u32 x,y;
	for(y = 1; y < SCR_LINES; y++)
	{
		for(x = 0; x < SCR_CHARS; x++)
		{
			SCR_TEXT_BUF[(y-1)*SCR_CHARS+x] 
				= SCR_TEXT_BUF[y*SCR_CHARS+x];
		}
	}
	
	for(x = 0; x < SCR_CHARS; x++)
	{
		SCR_TEXT_BUF[(SCR_LINES-1)*SCR_CHARS+x] 
			= SCR_ENTRY(0,DEFAULT_COLOR);
	}
}

void chg_color(u8 color)
{
	s_cCurAttr = color;
}

void print(const char* str)
{
	while(*str) printchar(*str++);
}

void move_to_next_char(){}
void move_to_next_line(){}
void locate(int h, int w)
{
	s_iX = w;
	s_iY = h;
}
