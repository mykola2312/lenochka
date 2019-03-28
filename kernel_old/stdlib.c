#include "stdlib.h"
#include "vga_terminal.h"
#include "keyboard.h"
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

static char* xnums = "0123456789ABCDEF";

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
			else if(c == '%')
			{
				if(cur != end) *cur++ = '%';
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
				case '%': printchar('%'); break;
			}
		}
		else printchar(c);
	}
}


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

//static char* katoi_base = "0123456789ABCDEF";

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
			if(*end == xnums[i]) break;
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
			if(*end == xnums[i]) break;
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
			if(c == xnums[i]) break;
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
	u32 len,maxLen;
	union {
		i32* iptr;
		u32* uptr;
		char* sptr;
	} u;
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
					u.iptr = (i32*)(*(u32*)argp++);
					*u.iptr = _katoi(buf,10,len);
					buf += len; //we decrement because it will +1
					break;
				case 'p':
				case 'x':
					buf += 2; //Skip 0x part
					len = _kstrchr_delim(buf,16);
					u.iptr = (i32*)(*(u32*)argp++);
					*u.iptr = _katoi(buf,16,len);
					buf += len; //we decrement because it will +1
					break;
				case 's':
					stk = kstrchr(buf,*(fmt+1));
					u.sptr = (char*)(*(u32*)argp++);
					maxLen = (*(u32*)argp++);
					if(!stk)
					{
						u.sptr[0] = '\0';
						break;
					}
					len = (u32)stk-(u32)buf;
					if(len > maxLen) len = maxLen;
					kmemcpy(u.sptr,buf,len);
					u.sptr[len] = '\0';
					buf += len;
					break;
			}
			
		} else buf++;
		fmt++;
	}
}
