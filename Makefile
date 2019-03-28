OLD_KOBJ=multiboot.o kmain.o vga_terminal.o stdlib.o x86stdlib.o		\
	gdt.o x86gdt.o pic.o idt.o x86idt.o x86int.o string.o		\
	paging.o x86utils.o x86_utils.o keyboard.o exceptions.o		\
	memory.o pci.o x86io.o pit.o ahci.o
	
KOBJ=start.o kmain.o x64io.o stdlib.o string.o vga_terminal.o	\
	x64stdlib.o va_list.o idt.o x64idt.o x64ints.o pic.o pit.o	\
	exceptions.o keyboard.o paging.o multiboot.o lvalloc.o		\
	pci.o ahci.o disk.o fat.o heap.o x64utils.o file.o			\
	x64font.o video.o bmp.o
LOBJ=loader.o lmain.o x86io.o x86stdlib.o x86gdt.o stdlib.o		\
	string.o vga_terminal.o gdt.o x86enter64.o memory.o	idt.o	\
	exceptions.o x86idt.o x86int.o

KSRC_DIR=kernel/
LSRC_DIR=loader/
KOBJ_DIR=kobj/
LOBJ_DIR=lobj/
INCLUDE=kernel/
LINCLUDE=loader/

TOOLCHAIN32=/home/adriane/opt/cross
HDISK=/home/adriane/lenochka_vm/hdisk.img
CC32=$(TOOLCHAIN32)/bin/i686-elf-gcc
AS32=nasm
LD32=$(TOOLCHAIN32)/i686-elf/bin/ld

TOOLCHAIN=/home/adriane/opt64/cross
CC=$(TOOLCHAIN)/bin/x86_64-elf-gcc
AS=nasm
LD=$(TOOLCHAIN)/x86_64-elf/bin/ld

# 32 bit
OPT_C32=-O0 -Wall -std=c99 -nostdlib -nostartfiles -nodefaultlibs 	\
	-Wimplicit-function-declaration -fno-stack-protector
OPT_ASM32=-s -f elf32
OPT_LD32=-T loader.ld --build-id=none -m elf_i386 -Map lmap.txt

# 64 bit
OPT_C=-m64 -ffreestanding -z max-page-size=0x1000 -mno-red-zone 	\
	-mno-mmx -mno-sse -mno-sse2 -std=gnu99 -O0 -Wall -Wextra		\
	-nostdlib -nostartfiles -nodefaultlibs -mcmodel=large
OPT_ASM=-s -f elf64
OPT_LD=-T lenochka.ld --build-id=none -m elf_x86_64 -Map kmap.txt	\
	-z max-page-size=0x1000

%.o: $(KSRC_DIR)%.c
	$(CC) $(OPT_C) -c $< -o $(KOBJ_DIR)$@ -I$(INCLUDE)
%.o: $(KSRC_DIR)%.asm
	$(AS) $(OPT_ASM) $< -o $(KOBJ_DIR)$@ 

loader.elf:
	$(AS32) $(OPT_ASM32) $(LSRC_DIR)loader.asm -o $(LOBJ_DIR)loader.o
	$(AS32) $(OPT_ASM32) $(LSRC_DIR)x86io.asm -o $(LOBJ_DIR)x86io.o
	$(AS32) $(OPT_ASM32) $(LSRC_DIR)x86stdlib.asm -o $(LOBJ_DIR)x86stdlib.o
	$(AS32) $(OPT_ASM32) $(LSRC_DIR)x86gdt.asm -o $(LOBJ_DIR)x86gdt.o
	$(AS32) $(OPT_ASM32) $(LSRC_DIR)x86idt.asm -o $(LOBJ_DIR)x86idt.o
	$(AS32) $(OPT_ASM32) $(LSRC_DIR)x86int.asm -o $(LOBJ_DIR)x86int.o
	
	$(CC32) $(OPT_C32) -c $(LSRC_DIR)stdlib.c -o $(LOBJ_DIR)stdlib.o -I$(LINCLUDE)
	$(CC32) $(OPT_C32) -c $(LSRC_DIR)gdt.c -o $(LOBJ_DIR)gdt.o -I$(LINCLUDE)
	$(CC32) $(OPT_C32) -c $(LSRC_DIR)vga_terminal.c -o $(LOBJ_DIR)vga_terminal.o -I$(LINCLUDE)
	$(CC32) $(OPT_C32) -c $(LSRC_DIR)string.c -o $(LOBJ_DIR)string.o -I$(LINCLUDE)
	$(CC32) $(OPT_C32) -c $(LSRC_DIR)memory.c -o $(LOBJ_DIR)memory.o -I$(LINCLUDE)
	$(CC32) $(OPT_C32) -c $(LSRC_DIR)idt.c -o $(LOBJ_DIR)idt.o -I$(LINCLUDE)
	$(CC32) $(OPT_C32) -c $(LSRC_DIR)exceptions.c -o $(LOBJ_DIR)exceptions.o -I$(LINCLUDE)
	$(CC32) $(OPT_C32) -c $(LSRC_DIR)lmain.c -o $(LOBJ_DIR)lmain.o -I$(LINCLUDE)
	
	$(AS32) $(OPT_ASM32) $(LSRC_DIR)x86enter64.asm -o $(LOBJ_DIR)x86enter64.o
	
	$(LD32) $(OPT_LD32) -o os/loader.elf $(addprefix $(LOBJ_DIR),$(LOBJ))

lenochka.elf: $(KOBJ)
	$(LD) $(OPT_LD) -o os/lenochka.elf $(addprefix $(KOBJ_DIR),$(KOBJ))

mkbootldr:
	nasm -f bin boot/stage1.asm -o boot/boot.bin
	nasm -f bin boot/stage2.asm -o os/BOOTLOAD.BIN
	
format:
	dd if=/dev/zero of=vm/hdisk.img bs=1M count=64
	mkdosfs -F 16 vm/hdisk.img
	
mount:
	losetup /dev/loop0 $(HDISK)
	losetup /dev/loop1 $(HDISK) -o 1048576
	losetup /dev/loop2 $(HDISK) -o 34603008
	mount -o umask=000 /dev/loop1 /media/img
	mount -o umask=000 /dev/loop2 /media/disk1

update:
	cp os/loader.elf /media/img/boot/loader.elf
	cp os/lenochka.elf /media/img/boot/lenochka.elf

unmount:
	umount /media/img
	umount /media/disk1
	losetup -d /dev/loop0
	losetup -d /dev/loop1
	losetup -d /dev/loop2

unmount1:
	losetup -d /dev/loop0
	losetup -d /dev/loop1
	losetup -d /dev/loop2

install-usb:
	mount -o umask=000 /dev/sdb1 /media/usb_3
	cp os/loader.elf /media/usb_3/boot/loader.elf
	cp os/lenochka.elf /media/usb_3/boot/lenochka.elf
	umount /media/usb_3

run-qemu:
	qemu-system-x86_64 -hda $(HDISK) -m 512M -machine q35
	
run-qemu-debug:
	qemu-system-x86_64 -hda $(HDISK) -m 512M -machine q35 -d int	\
		-no-shutdown -no-reboot -d guest_errors

clean:
	rm -rf kobj/*
	rm -rf lobj/*

all: lenochka.elf
