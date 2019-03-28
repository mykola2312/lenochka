#ifndef __VA_LIST_H
#define __VA_LIST_H

#include "os.h"

typedef struct {
	u64 args[6];
} va_list;

extern void va_start(va_list*);
extern u64 va_arg(va_list*,u64);

#endif
