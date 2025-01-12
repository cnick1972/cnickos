CC			:= clang
LD 			:= ld.lld
ASM 		:= nasm
MAKE		:= make

BUILDDIR    := ./build
MBRBIR      := ./src/bootloader/mbr
STAGE1DIR	:= ./src/bootloader/stage1
STAGE2DIR	:= ./src/bootloader/stage2
KERNELDIR	:= ./src/kernel

STAGE2_SRC :=  $(shell find $(STAGE2DIR) -name '*.asm' -or -name '*.c')
STAGE2_OBJS := $(STAGE2_SRC:%=$(BUILDDIR)/%.o)

KERNEL_SRC := $(shell find $(KERNELDIR) -name '*.asm' -or -name '*.c')
KERNEL_OBJS := $(KERNEL_SRC:%=$(BUILDDIR)/%.o)

CCFLAGS := -std=c99 -c -ffreestanding -target i686-none-elf -I./src/kernel -I./src/libs
LINKFLAGS := -L. -nostdlib --oformat binary

all: $(BUILDDIR)/mbr.bin $(BUILDDIR)/stage1.bin $(BUILDDIR)/stage2.bin $(BUILDDIR)/kernel.bin
	@echo "Building disk image"
	@dd if=$(BUILDDIR)/mbr.bin of=image/os.img conv=notrunc >/dev/null 2>&1
	@dd if=$(BUILDDIR)/stage1.bin of=image/os.img seek=2048 conv=notrunc >/dev/null 2>&1
	@mcopy -o $(BUILDDIR)/bootloader/stage2/stage2.bin c:
	@mcopy -o $(BUILDDIR)/kernel/kernel.bin c:

$(BUILDDIR)/mbr.bin: $(MBRBIR)/mbr.asm
	@echo "ASM " $<
	@mkdir -p $(BUILDDIR)
	@$(ASM) -f bin -o $@ $<

$(BUILDDIR)/stage1.bin: $(STAGE1DIR)/stage1.asm
	@echo "ASM " $<
	@mkdir -p $(BUILDDIR)
	@$(ASM) -f bin -o $@ $<

$(BUILDDIR)/stage2.bin:
	$(MAKE) -C src/bootloader/stage2

$(BUILDDIR)/kernel.bin:
	$(MAKE) -C src/kernel


.PHONY: build clean test

test:
	@qemu-system-i386 -drive file=image/os.img,index=0,media=disk,format=raw

.PHONY: build clean test

clean:
	rm -rfv ./build




