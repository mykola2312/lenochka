#include "stdlib.h"
#include "vga_terminal.h"
#include "keyboard.h"
#include "string.h"
#include "va_list.h"

void iprint(i64 number)
{
	//18446744073709551615
	char num[32];
	char* s = num;
	int uns = 0;
	
	if(number < 0)
	{
		uns = 1;
		number *= -1;
	}
	while(number >= 10)
	{
		*s++ = '0'+(number%10);
		number/=10;
	}
	*s++ = '0'+(number%10);
	if(uns) *s++ = '-';
	s--;
	while(s >= num) printchar(*s--);
}

void uprint(u64 number)
{
	//18446744073709551615
	char num[32];
	char* s = num;
	while(number >= 10)
	{
		*s++ = '0'+(number%10);
		number/=10;
	}
	*s++ = '0'+(number%10);
	s--;
	while(s >= num) printchar(*s--);
}

static char* xnums = "0123456789ABCDEF";

void xprint(u64 number)
{
	//FFFFFFFFFFFFFFFF
	char num[32];
	char* s = num;
	
	while(s-num < 15)
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

void fprint(u64 number)
{
	u64 val;
	char num[64];
	int i;
	
	//decimal
	i = 0;
	val = number>>16;
	while(val != 0)
	{
		num[i++] = '0'+(val%10);
		val /= 10;
	}
	num[i++] = '0'+(val%10);
	while(i != 0) printchar(num[--i]);
	
	//fraction
	i = 0;
	val = number&0xFFFF;
	printchar('.');
	while(val != 0)
	{
		val *= 10;
		printchar('0'+(val>>16));
		val &= 0xFFFF;
	}
}

void iprints(char* outs,i64 number)
{
	//18446744073709551615
	char num[32];
	char* s = num;
	int uns = 0;
	
	if(number < 0)
	{
		uns = 1;
		number *= -1;
	}
	while(number >= 10)
	{
		*s++ = '0'+(number%10);
		number/=10;
	}
	*s++ = '0'+(number%10);
	if(uns) *s++ = '-';
	while(s >= num) *outs++ = *--s;
	*outs++ = '\0';
}

void uprints(char* outs,u64 number)
{
	//18446744073709551615
	char num[32];
	char* s = num;
	while(number >= 10)
	{
		*s++ = '0'+(number%10);
		number/=10;
	}
	*s++ = '0'+(number%10);
	while(s >= num) *outs++ = *--s;
	*outs++ = '\0';
}

void xprints(char* outs,u64 number)
{
	//FFFFFFFFFFFFFFFF
	char num[32];
	char* s = num;
	
	while(s-num < 15)
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
	print(str);
}

void ksprintf(char* dst,size_t maxLen,const char* fmt,...)
{
	va_list ap;
	va_start(&ap);
	
	u64 idx;
	char* cur,*end,c;
	char numbuf[64];
	
	idx = 1;
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
				iprints(numbuf,(ilong)va_arg(&ap,idx++));
				while((*cur++ = *p++))
					if(cur == end) break;
				cur--;
			}
			else if(c == 'u')
			{
				uprints(numbuf,(ilong)va_arg(&ap,idx++));
				while((*cur++ = *p++))
					if(cur == end) break;
				cur--;
			}
			else if(c == 'p' || c == 'x')
			{
				xprints(numbuf,(ulong)va_arg(&ap,idx++));
				while((*cur++ = *p++))
					if(cur == end) break;
				cur--;
			}
			else if(c == 's')
			{
				const char* str = (const char*)va_arg(&ap,idx++);
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
	va_list ap;
	va_start(&ap);
	
	u64 idx;
	dbcs_t c;
	
	idx = 1;
	//while((c = *fmt++))
	do
	{
		c = kchr2dbcs(fmt);
		if(!c) break;
		fmt += kchrlen(*fmt);
		if(c == '%')
		{
			//c = *fmt++;
			c = kchr2dbcs(fmt);
			fmt += kchrlen(*fmt);
			switch(c)
			{
				case 'd': iprint((i64)va_arg(&ap,idx++)); break;
				case 'u': uprint((u64)va_arg(&ap,idx++)); break;
				case 'p':
				case 'x': xprint((u64)va_arg(&ap,idx++)); break;
				case 'c': printchar((char)(va_arg(&ap,idx++))); break;
				case 's': kprint((char*)(va_arg(&ap,idx++))); break;
				case 'f': fprint((u64)(va_arg(&ap,idx++))); break;
				case '%': printchar('%'); break;
			}
		}
		else printchar(c);
	} while(c);
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

dbcs_t kgetc()
{
	dbcs_t ascii;
	while(keyboard_scan_code() == 0)
	{__asm("hlt");}
	ascii = keyboard_sym();
	keyboard_reset_irq1_vars();
	return ascii;
}

void kgets(dbcs_t* buf,size_t cMax)
{
	dbcs_t ch;
	dbcs_t* ptr,*end;
	
	ptr = buf;
	end = buf + cMax;
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

static i64 _katoi(char* str,int base,size_t len)
{
	char* end;
	i64 i,pow;
	i64 num;
	
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

static u64 _katou(char* str,int base,u64 len)
{
	char* end;
	u64 pow,num;
	i64 i;
	
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

i64 katoi(char* str,int base)
{
	return _katoi(str,base,kstrlen(str));
}

u64 katou(char* str,int base)
{
	return _katou(str,base,kstrlen(str));
}

static u64 _kstrchr_delim(char* buf,u32 base)
{
	u64 i,len;
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
	//ulong* argp;
	va_list ap;
	va_start(&ap);
	
	u64 idx;
	ulong len,maxLen;
	union {
		i64* iptr;
		u64* uptr;
		char* sptr;
	} u;
	char c,*stk;
	
	idx = 2;
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
					u.iptr = (i64*)(va_arg(&ap,idx++));
					*u.iptr = _katoi(buf,10,len);
					buf += len; //we decrement because it will +1
					break;
				case 'p':
				case 'x':
					buf += 2; //Skip 0x part
					len = _kstrchr_delim(buf,16);
					u.iptr = (i64*)(va_arg(&ap,idx++));
					*u.iptr = _katoi(buf,16,len);
					buf += len; //we decrement because it will +1
					break;
				case 's':
					stk = (char*)kstrchr(buf,*(fmt+1));
					u.sptr = (char*)(va_arg(&ap,idx++));
					maxLen = (ulong)(va_arg(&ap,idx++));
					if(!stk)
					{
						u.sptr[0] = '\0';
						break;
					}
					len = (u64)stk-(u64)buf;
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
