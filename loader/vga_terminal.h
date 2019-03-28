#ifndef __VGA_TERMINAL_H
#define __VGA_TERMINAL_H

#include "os.h"

#define SCR_CHARS 80
#define SCR_LINES 25
//#define SCR_TEXT_BUF ((u16*)0xB8000)
#define SCR_TEXT_BUF ((u16*)OS_VIDEO_BUFFER)
#define SCR_ENTRY(ch,col) ((col<<8)|ch)

#define BLACK  0
#define BLUE  1
#define GREEN  2
#define CYAN  2
#define RED 4
#define GRAY 8
#define WHITE 15
#define MAGENTA 13

#define BG_BLACK  (0<<4)
#define BG_BLUE  (1<<4)
#define BG_GREEN  (2<<4)
#define BG_RED (4<<4)
#define BG_GRAY (8<<4)
#define BG_WHITE (15<<4)

#define DEFAULT_COLOR BG_BLACK|7

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline u8 vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline u16 vga_entry(unsigned char uc, u8 color) {
	return (u16) uc | (u16) color << 8;
}

void vga_terminal_init();

void cls();
void shiftscreen();
void printchar(unsigned char c);
void chg_color(u8 color);
void print(const char* str);
void locate(int h, int w);


#endif
