#ifndef __VIDEO_H
#define __VIDEO_H

#include "os.h"

void video_pixel(u64 x,u64 y,u32 color);
void video_char(u64 x,u64 y,u64 sizew,u64 sizeh,u32 color,u16 sym);
void video_rect_zero(u64 x,u64 y,u64 w,u64 h);
void video_line_copy(u64 ysrc,u64 ydst,u64 height);
void video_line_zero(u64 y,u64 height);
void video_clear();

void video_swap();
void video_init();

#endif
