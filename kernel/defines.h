#ifndef __DEFINES_H
#define __DEFINES_H

#define ROUND2_UP(val,bit) (( (val>>bit) 	\
	+ ( (val&((1<<bit)-1)) ? 1 : 0 ) ) << bit)

#define ROUND1_ADD(val) (val ? val : 1)
#define MIN(a,b) (a < b ? a : b)

#endif
