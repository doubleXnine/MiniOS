#########################
# Makefile for Orange'S #
#########################

# Entry point of Orange'S
# It must have the same value with 'KernelEntryPointPhyAddr' in load.inc!
#############edit by visual 2016.5.10####
ENTRYPOINT	= 0xC0030400


# Offset of entry point in kernel file
# It depends on ENTRYPOINT
ENTRYOFFSET	=   0x400

# Programs, flags, etc.
ASM		= nasm
DASM	= ndisasm
CC		= gcc
LD		= ld
AR		= ar

ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
DASMFLAGS	= -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)
ARFLAGS		= rcs

# CFLAGS		= -I include/ -c -fno-builtin -fno-stack-protector
# CFLAGS		= -I include/ -m32 -c -fno-builtin -fno-stack-protector
#modified by xw
CFLAGS		= -I include/ -m32 -c -fno-builtin -fno-stack-protector -Wall -Wextra -Wfatal-errors -g
CFLAGS_app	= -I include/ -m32 -c -fno-builtin -Wall -Wextra -Wfatal-errors -g

# LDFLAGS		= -s -Ttext $(ENTRYPOINT)
# LDFLAGS		= -m elf_i386 -s -Ttext $(ENTRYPOINT)
#generate map file. added by xw
LDFLAGS_kernel	= -m elf_i386 -s -Ttext $(ENTRYPOINT) -Map kernel.map
LDFLAGS_init	= -m elf_i386 -s -Map init/init.map
#discard -s, so keep symbol information that gdb can use. added by xw
LDFLAGS_kernel_gdb	= -m elf_i386 -Ttext $(ENTRYPOINT)
LDFLAGS_init_gdb	= -m elf_i386

# This Program
ORANGESBOOT	= boot/boot.bin boot/loader.bin
ORANGESKERNEL	= kernel.bin
ORANGESINIT	= init/init.bin
OBJS		= kernel/kernel.o kernel/syscall.o kernel/start.o kernel/main.o kernel/clock.o\
			kernel/i8259.o kernel/global.o kernel/protect.o kernel/proc.o\
			lib/kliba.o lib/klib.o lib/string.o kernel/syscallc.o kernel/memman.o kernel/pagetbl.o	\
			kernel/elf.o kernel/file.o kernel/exec.o kernel/fork.o kernel/pthread.o \
			kernel/ktest.o kernel/testfunc.o kernel/fs.o kernel/hd.o
OBJSINIT	= init/init.o init/initstart.o lib/ulib.a 
OBJSULIB = lib/string.o kernel/syscall.o
DASMOUTPUT	= kernel.bin.asm
#added by xw
GDBBIN = kernel.gdb.bin init/init.gdb.bin

# All Phony Targets
.PHONY : everything final image clean realclean disasm all buildimg

# Default starting position
nop :
	@echo "why not \`make image' huh? :)"

everything : $(ORANGESBOOT) $(ORANGESKERNEL) $(ORANGESINIT) $(GDBBIN)

all : realclean everything

# image : realclean everything clean buildimg
image : everything buildimg

clean :
	rm -f $(OBJS) $(OBJSINIT)

realclean :
	rm -f $(OBJS) $(OBJSINIT) $(ORANGESBOOT) $(ORANGESKERNEL) $(ORANGESINIT) $(GDBBIN)

disasm :
	$(DASM) $(DASMFLAGS) $(ORANGESKERNEL) > $(DASMOUTPUT)

# We assume that "a.img" exists in current folder
buildimg :
	dd if=boot/boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img /mnt/floppy/
	sudo cp -fv boot/loader.bin /mnt/floppy/
	sudo cp -fv kernel.bin /mnt/floppy
	sudo cp -fv init/init.bin /mnt/floppy
	sudo cp -fv command/echo.bin /mnt/floppy
	sudo umount /mnt/floppy

# compress 80m.img. added by xw, 18/6/17
gz80m :
	tar zcvf 80m.img.tar.gz 80m.img

boot/boot.bin : boot/boot.asm boot/include/load.inc boot/include/fat12hdr.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader.bin : boot/loader.asm boot/include/load.inc boot/include/fat12hdr.inc boot/include/pm.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(ORANGESKERNEL) : $(OBJS)
#	$(LD) $(LDFLAGS) -o $(ORANGESKERNEL) $(OBJS)
#modified by xw, 18/6/10
#	$(LD) $(LDFLAGS) -Map kernel.map -o $(ORANGESKERNEL) $(OBJS)
#modified by xw, 18/6/12
	$(LD) $(LDFLAGS_kernel) -o $(ORANGESKERNEL) $(OBJS)
	
$(ORANGESINIT) : $(OBJSINIT)
#	$(LD) -s -o $(ORANGESINIT) $(OBJSINIT)
#	$(LD) -m elf_i386 -s -o $(ORANGESINIT) $(OBJSINIT)
#modified by xw, 18/6/11
#	$(LD) -m elf_i386 -s -Map init/init.map -o $(ORANGESINIT) $(OBJSINIT)
#modified by xw, 18/6/12
	$(LD) $(LDFLAGS_init) -o $(ORANGESINIT) $(OBJSINIT)

#added by xw
kernel.gdb.bin : $(OBJS)
	$(LD) $(LDFLAGS_kernel_gdb) -o $@ $(OBJS)
init/init.gdb.bin : $(OBJSINIT)
	$(LD) $(LDFLAGS_init_gdb) -o $@ $(OBJSINIT)
	
kernel/kernel.o : kernel/kernel.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/syscall.o : kernel/syscall.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/start.o: kernel/start.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/main.o: kernel/main.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/clock.o: kernel/clock.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/i8259.o: kernel/i8259.c include/type.h include/const.h include/protect.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o: kernel/global.c include/type.h include/const.h include/protect.h include/proc.h \
			include/global.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/protect.o: kernel/protect.c include/type.h include/const.h include/protect.h include/proc.h include/proto.h \
			include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/proc.o: kernel/proc.c
	$(CC) $(CFLAGS) -o $@ $<

lib/klib.o: lib/klib.c include/type.h include/const.h include/protect.h include/string.h include/proc.h include/proto.h \
			include/global.h
	$(CC) $(CFLAGS) -o $@ $<

lib/kliba.o : lib/kliba.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/string.o : lib/string.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/syscallc.o: kernel/syscallc.c include/type.h include/const.h include/protect.h include/proto.h \
			include/string.h include/proc.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<	

kernel/memman.o: kernel/memman.c /usr/include/stdc-predef.h include/memman.h include/type.h include/const.h include/protect.h \
 			include/proto.h include/string.h include/proc.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<
	
kernel/pagetbl.o: kernel/pagetbl.c include/type.h include/const.h include/protect.h include/proto.h include/string.h \
			include/proc.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<	

lib/ulib.a:  $(OBJSULIB)
	$(AR) $(ARFLAGS) -o $@  $(OBJSULIB)
	
init/init.o: init/init.c include/stdio.h
	$(CC) $(CFLAGS_app) -o $@ $<
	
init/initstart.o: init/initstart.asm 
	$(ASM) $(ASMKFLAGS) -o $@ $<
	
kernel/elf.o: kernel/elf.c /usr/include/stdc-predef.h include/type.h include/const.h include/protect.h \
			include/proto.h include/string.h include/proc.h include/global.h include/elf.h
	$(CC) $(CFLAGS) -o $@ $<
	
kernel/file.o: kernel/file.c /usr/include/stdc-predef.h include/type.h include/const.h include/protect.h \
			include/proto.h include/string.h include/proc.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<
	
kernel/exec.o: kernel/exec.c /usr/include/stdc-predef.h include/type.h include/const.h include/protect.h \
			include/proto.h include/string.h include/proc.h include/global.h include/elf.h
	$(CC) $(CFLAGS) -o $@ $<
	
kernel/fork.o: kernel/fork.c /usr/include/stdc-predef.h include/type.h include/const.h include/protect.h \
			include/proto.h include/string.h include/proc.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<
	
kernel/pthread.o: kernel/pthread.c /usr/include/stdc-predef.h include/type.h include/const.h include/protect.h \
			include/proto.h include/string.h include/proc.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/ktest.o: kernel/ktest.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/proto.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/testfunc.o: kernel/testfunc.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/proto.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<
	
# fs.o and hd.o; added by xw, 18/5/25
kernel/fs.o: kernel/fs.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/proto.h include/global.h include/fs_const.h include/fs.h include/hd.h include/fs_misc.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/hd.o: kernel/hd.c include/type.h include/const.h include/protect.h include/string.h include/proc.h \
			include/proto.h include/global.h include/fs_const.h include/fs.h include/hd.h include/fs_misc.h
	$(CC) $(CFLAGS) -o $@ $<
