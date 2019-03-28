#include "termlib.h"
#include "vga_terminal.h"
#include "string.h"

void iprint(int number)
{
	//4294967295
	char num[32];
	char* s = num;
	int uns = 0;
	
	if(number < 0)
	{
		uns = 1;
		number *= -1;
	}
	while(number > 10)
	{
		*s++ = '0'+(number%10);
		number/=10;
	}
	*s++ = '0'+(number%10);
	if(uns) *s++ = '-';
	s--;
	while(s >= num) printchar(*s--);
}

void uprint(u32 number)
{
	//4294967295
	char num[32];
	char* s = num;
	while(number > 10)
	{
		*s++ = '0'+(number%10);
		number/=10;
	}
	*s++ = '0'+(number%10);
	s--;
	while(s >= num) printchar(*s--);
}

const char* xnums = "0123456789ABCDEF";

void xprint(u32 number)
{
	//FFFFFFFF
	char num[16];
	char* s = num;
	
	while(s-num < 7)
	{
		*s++ = xnums[(number%16)];
		number/=16;
	}
	*s++ = xnums[(number%16)];
	*s++ = 'x';
	*s++ = '0';
	s--;
	while(s >= num) printchar(*s--);
}

void iprints(char* outs,int number)
{
	//4294967295
	char num[32];
	char* s = num;
	int uns = 0;
	
	if(number < 0)
	{
		uns = 1;
		number *= -1;
	}
	while(number > 10)
	{
		*s++ = '0'+(number%10);
		number/=10;
	}
	*s++ = '0'+(number%10);
	if(uns) *s++ = '-';
	while(s >= num) *outs++ = *--s;
	*outs++ = '\0';
}

void uprints(char* outs,u32 number)
{
	//4294967295
	char num[32];
	char* s = num;
	while(number > 10)
	{
		*s++ = '0'+(number%10);
		number/=10;
	}
	*s++ = '0'+(number%10);
	while(s >= num) *outs++ = *--s;
	*outs++ = '\0';
}

void xprints(char* outs,u32 number)
{
	//FFFFFFFF
	char num[16];
	char* s = num;
	
	while(s-num < 7)
	{
		*s++ = xnums[(number%16)];
		number/=16;
	}
	*s++ = xnums[(number%16)];
	*s++ = 'x';
	*s++ = '0';
	while(s >= num) *outs++ = *--s;
	*outs++ = '\0';
}

void kprint(char* str)
{
	while(*str) printchar(*str++);
}

void ksprintf(char* dst,u32 maxLen,const char* fmt,...)
{
	u32* argp;
	char* cur,*end,c;
	char numbuf[64];
	
	argp = ((u32*)&fmt)+1;
	cur = dst;
	end = dst+maxLen;
	while((c = *fmt++) && cur != end)
	{
		if(c == '%')
		{
			char* p = numbuf;
			c = *fmt++;
			if(c == 'd')
			{	
				iprints(numbuf,*(int*)argp++);
				while((*cur++ = *p++))
					if(cur == end) break;
				cur--;
			}
			else if(c == 'u')
			{
				uprints(numbuf,*(int*)argp++);
				while((*cur++ = *p++))
					if(cur == end) break;
				cur--;
			}
			else if(c == 'p' || c == 'x')
			{
				xprints(numbuf,*(u32*)argp++);
				while((*cur++ = *p++))
					if(cur == end) break;
				cur--;
			}
			else if(c == 's')
			{
				const char* str = *(const char**)argp++;
				while((*cur++ = *str++))
					if(cur == end) break;
				cur--;
			}
		}
		else
		{
			if(cur == end) break;
			*cur++ = c;
		}
	}
	if(cur == end)
		*(end-1) = '\0';
}

void kprintf(const char* fmt,...)
{
	u32* argp;
	char c;
	
	argp = ((u32*)&fmt)+1;
	while((c = *fmt++))
	{
		if(c == '%')
		{
			c = *fmt++;
			switch(c)
			{
				case 'd': iprint(*(int*)argp++); break;
				case 'u': uprint(*(u32*)argp++); break;
				case 'p':
				case 'x': xprint(*(u32*)argp++); break;
				case 'c': printchar((char)(*(u32*)argp++)); break;
				case 's': kprint((char*)(*(u32*)argp++)); break;
			}
		}
		else printchar(c);
	}
}
