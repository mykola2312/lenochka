#ifndef __STRING_H
#define __STRING_H

#include "os.h"

u32 kstrlen(const char* str);

void kstrcpy(char* dst,const char* src);
void kstrncpy(char* dst,const char* src,u32 maxLen);

char* kstrchr(char* src,char chr);
char* kstrrchr(char* src,char chr);

int kstrncmp(const char* haystack,char* needle);

#endif
