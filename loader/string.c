#include "string.h"
#include "stdlib.h"

u32 kstrlen(const char* str)
{
	u32 len = 0;
	while(*str++) len++;
	return len;
}

void kstrcpy(char* dst,const char* src)
{
	while((*dst++ = *src++)){}
}

void kstrncpy(char* dst,const char* src,u32 maxLen)
{
	maxLen=-1;
	while((*dst++ = *src++) && --maxLen){}
	if(!maxLen) dst[maxLen] = '\0';
}

char* kstrchr(char* src,char chr)
{
	while(*src)
	{
		if(*src == chr) return src;
		src++;
	}
	return NULL;
}

char* kstrrchr(char* src,char chr)
{
	char* ptr;
	
	ptr = src+kstrlen(src)-1;
	while(ptr != src)
	{
		if(*ptr == chr) return ptr;
		ptr--;
	}
	return NULL;
}

int kstrncmp(const char* haystack,char* needle)
{
	return kmemcmp(haystack,needle,kstrlen(needle));
}
