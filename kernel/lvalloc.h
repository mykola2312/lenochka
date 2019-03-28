#ifndef __LVALLOC_H
#define __LVALLOC_H

#include "os.h"

void* lvalloc(size_t size);

void lvalloc_push();
void lvalloc_pop();

void* lvalloc_get_next_addr();

void lvalloc_init();

#endif
