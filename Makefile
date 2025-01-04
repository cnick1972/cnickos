CC			:= clang
LD 			:= ld.lld
ASM 		:= nasm

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
	@mcopy -o $(BUILDDIR)/stage2.bin c:
	@mcopy -o $(BUILDDIR)/kernel.bin c:

$(BUILDDIR)/mbr.bin: $(MBRBIR)/mbr.asm
	@echo "ASM " $<
	@mkdir -p $(BUILDDIR)
	@$(ASM) -f bin -o $@ $<

$(BUILDDIR)/stage1.bin: $(STAGE1DIR)/stage1.asm
	@echo "ASM " $<
	@mkdir -p $(BUILDDIR)
	@$(ASM) -f bin -o $@ $<

$(BUILDDIR)/stage2.bin: $(STAGE2_OBJS)
	@echo "LD  " $@
	@ld.lld -T $(STAGE2DIR)/linker.ld $(LINKFLAGS) -Map=$(BUILDDIR)/stage2.map -o $@ $(STAGE2_OBJS) -lgcc

$(BUILDDIR)/kernel.bin: $(KERNEL_OBJS)
	@echo "LD  " $@
	@ld.lld -T $(KERNELDIR)/linker.ld $(LINKFLAGS) -Map=$(BUILDDIR)/kernel.map -o $@ $(KERNEL_OBJS) -lgcc

$(BUILDDIR)/%.c.o: %.c
	@echo "CC  " $<
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -o $@ $<

$(BUILDDIR)/%.asm.o: %.asm
	@echo "ASM " $<
	@mkdir -p $(dir $@)
	@$(ASM) $< -f elf -o $@

.PHONY: build clean

clean:
	rm -rfv ./build




