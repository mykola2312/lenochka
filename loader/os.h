#ifndef __OS_H
#define __OS_H

#define OS_NULL_SELECTOR 0x00
#define OS_CODE_SELECTOR 0x08
#define OS_DATA_SELECTOR 0x10

#define OS_PHYS_MAPPING 0x00100000
#define OS_VIRT_MAPPING 0xC0000000
#define OS_PM86_MEM_SIZE_4K (1024*1024)
#define OS_PHYS_ADDR(virt) (virt-OS_VIRT_MAPPING)

#define OS_VIDEO_BUFFER 0x000B8000
#define OS_PROG_MAPPING 0x01000000

#define __packed __attribute__((packed))
#define __align(N) __attribute__((aligned(N)))

#define __phys
#define __virt

#define NULL (void*)0

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef char i8;
typedef short i16;
typedef long i32;
typedef long long i64;

typedef unsigned long size_t;

#endif
