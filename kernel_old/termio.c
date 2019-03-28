#include "termio.h"
#include "keyboard.h"
#include "vga_terminal.h"
#include "string.h"

u8 kgetch()
{
	u8 key;
	while(keyboard_scan_code() == 0)
	{__asm("hlt");}
	key = keyboard_scan_code();
	keyboard_reset_irq1_vars();
	return key;
}

u8 kgetc()
{
	u8 ascii;
	while(keyboard_scan_code() == 0)
	{__asm("hlt");}
	ascii = keyboard_ascii();
	keyboard_reset_irq1_vars();
	return ascii;
}

void kgets(char* buf,size_t max)
{
	u8 ch;
	char* ptr,*end;
	
	ptr = buf;
	end = buf + max;
	*ptr = '\0';
	while((ch = kgetc()) != '\n')
	{
		if(ch == '\b')
		{
			if(ptr != buf) *ptr-- = '\0';
			printchar('\b');
		}
		else if(ptr != end-1)
		{
			*ptr++ = ch;
			printchar(ch);
		}
	}
	printchar('\n');
	if(ptr == end) ptr--;
	*ptr = '\0';
}

static char* katoi_base = "0123456789ABCDEF";

static i32 _katoi(char* str,int base,u32 len)
{
	char* end;
	i32 i,pow;
	i32 num;
	
	i = 0;
	num = 0;
	pow = 1;
	end = str+len-1;
	while(1)
	{
		if(*end == '-')
		{
			num *= -1;
			break;
		}
		else if(*end == 'x') break;
		for(i = 0; i < base; i++)
			if(*end == katoi_base[i]) break;
		num += i*pow;
		pow *= base;
		
		if(end == str) break;
		end--;
	}
	
	return num;
}

static u32 _katou(char* str,int base,u32 len)
{
	char* end;
	u32 i,pow;
	u32 num;
	
	i = 0;
	num = 0;
	pow = 1;
	end = str+len-1;
	while(1)
	{
		if(*end == 'x') break;
		for(i = 0; i < base; i++)
			if(*end == katoi_base[i]) break;
		num += i*pow;
		pow *= base;
		
		if(end == str) break;
		end--;
	}
	
	return num;
}

i32 katoi(char* str,int base)
{
	return _katoi(str,base,kstrlen(str));
}

u32 katou(char* str,int base)
{
	return _katou(str,base,kstrlen(str));
}

static u32 _kstrchr_delim(char* buf,u32 base)
{
	u32 i,len;
	char c;
	
	len = 0;
	while(*buf)
	{
		c = *buf;
		for(i = 0; i < base; i++) 
		{
			if(c == katoi_base[i]) break;
		}
		if(i == base && c != '-') break;
		buf++;
		len++;
	}
	return len;
}

void ksscanf(char* buf,char* fmt,...)
{
	u32* argp;
	u32 len;
	i32* iptr;
	u32* uptr;
	char c,*stk;
	
	argp = ((u32*)&fmt)+1;
	while((c = *fmt))
	{
		//kprintf("%c %c\n",c,*buf);
		if(c == '%')
		{
			c = *++fmt;
			switch(c)
			{
				case 'u':
				case 'd':
					len = _kstrchr_delim(buf,10);
					iptr = (i32*)(*(u32*)argp++);
					*iptr = _katoi(buf,10,len);
					buf += len; //we decrement because it will +1
					break;
				case 'p':
				case 'x':
					buf += 2; //Skip 0x part
					len = _kstrchr_delim(buf,16);
					iptr = (i32*)(*(u32*)argp++);
					*iptr = _katoi(buf,16,len);
					buf += len; //we decrement because it will +1
			}
			
		} else buf++;
		fmt++;
	}
}
