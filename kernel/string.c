#include "string.h"
#include "stdlib.h"

size_t kstrlen(const char* str)
{
	u32 len = 0;
	while(*str++) len++;
	return len;
}

void kstrcpy(char* dst,const char* src)
{
	while((*dst++ = *src++)){}
}

void kstrncpy(char* dst,const char* src,size_t maxLen)
{
	maxLen=-1;
	while((*dst++ = *src++) && --maxLen){}
	if(!maxLen) dst[maxLen] = '\0';
}

const char* kstrchr(const char* src,char chr)
{
	while(*src)
	{
		if(*src == chr) return src;
		src++;
	}
	return NULL;
}

const char* kstrrchr(const char* src,char chr)
{
	const char* ptr;
	
	ptr = src+kstrlen(src)-1;
	while(ptr != src)
	{
		if(*ptr == chr) return ptr;
		ptr--;
	}
	return NULL;
}

int kstrcmp(const char* src,const char* dst)
{
	if(kstrlen(src) != kstrlen(dst)) return 1;
	while(*src)
		if(*src++ != *dst++) return 1;
	return 0;
}

int kstrncmp(const char* haystack,char* needle)
{
	return kmemcmp(haystack,needle,kstrlen(needle));
}

int kstrcasecmp(const char* src,const char* dst)
{
	if(kstrlen(src) != kstrlen(dst)) return 1;
	while(*src)
	{
		kprintf("%c %c\n",ktolower(*src),ktolower(*dst));
		if(ktolower(*src++) != ktolower(*dst++)) return 1;
	}
	return 0;
}

char ktoupper(char c)
{
	if(c >= 0x61 && c <= 0x7A) return c&0xDF;
	return c;
}

char ktolower(char c)
{
	if(c >= 0x41 && c <= 0x5A) return c|(1<<5);
	return c;
}

size_t kchrlen(char cchr)
{
	u8 chr = (u8)cchr;
	
	if(chr & 0b10000000)
	{
		if((chr>>5) == 0b110) return 2;
		else if((chr>>4) == 0b1110) return 3;
		else if((chr>>5) == 0b11110) return 4;
	}
	return 1;
}

dbcs_t kchr2dbcs(const char* ptr)
{
	const u8* chr = (u8*)ptr;
	
	switch(kchrlen(chr[0]))
	{
		case 1: return chr[0]&0x7F;
		case 2: return ((chr[0]&0x1F)<<6)|(chr[1]&0x3F);
		case 3: return ((chr[0]&0x0F)<<12)|((chr[1]&0x3F)<<6)|(chr[2]&0x3F);
		default: return 0x003F;
	}
}

size_t kdbcslen(dbcs_t chr)
{
	if(chr > 0x7FF) return 3;
	else if(chr > 0x7F) return 2;
	return 1;
}

void kdbcs2str(dbcs_t* dbcs,char* str,size_t maxStr)
{
	char* end = str + maxStr - 1;
	dbcs_t wc;
	
	while((wc = *dbcs++) && str != end)
	{
		size_t len = kdbcslen(wc);
		if((str+len) >= end) break;
		switch(len)
		{
			case 1:
				*str++ = wc&0x7F;
				break;
			case 2:
				*str++ = 0b11000000|((wc>>6)&0x1F);
				*str++ = 0b10000000|((wc)&0x3F);
				break;
			case 3:
				*str++ = 0b11100000|((wc>>12)&0x0F);
				*str++ = 0b10000000|((wc>>6)&0x3F);
				*str++ = 0b10000000|((wc)&0x3F);
				break;
			default: *str++ = '?';
		}
	}
	*str++ = '\0';
}
