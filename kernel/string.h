#ifndef __STRING_H
#define __STRING_H

#include "os.h"

#define DBCS2SBCS(dbcs) ((char)(dbcs&0xFF))

typedef u16 dbcs_t;

size_t kstrlen(const char* str);

void kstrcpy(char* dst,const char* src);
void kstrncpy(char* dst,const char* src,size_t maxLen);

const char* kstrchr(const char* src,char chr);
const char* kstrrchr(const char* src,char chr);

int kstrcmp(const char* src,const char* dst);
int kstrncmp(const char* haystack,char* needle);
int kstrcasecmp(const char* src,const char* dst);

char ktoupper(char c);
char ktolower(char c);

size_t kchrlen(char chr);
dbcs_t kchr2dbcs(const char* chr);

size_t kdbcslen(dbcs_t chr);
void kdbcs2str(dbcs_t* dbcs,char* str,size_t maxStr);

#endif
