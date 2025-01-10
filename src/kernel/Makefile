CC			:= clang
LD 			:= ld.lld
ASM 		:= nasm

CCFLAGS := -std=c99 -c -ffreestanding -target i686-none-elf -I. -I../src/libs
LINKFLAGS := -L. -L/home/chris/cnickos/ -nostdlib --oformat binary

BUILD_DIR := ../../build/kernel

KERNELDIR := .
KERNEL_SRC := $(shell find $(KERNELDIR) -name '*.asm' -or -name '*.c')
KERNEL_OBJS := $(KERNEL_SRC:%=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(KERNEL_OBJS)
	@echo "LD  " $@
	@ld.lld -T $(KERNELDIR)/linker.ld $(LINKFLAGS) -Map=$(BUILD_DIR)/kernel.map -o $@ $(KERNEL_OBJS) -lgcc

$(BUILD_DIR)/%.c.o: %.c
	@echo "CC  " $<
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -o $@ $<

$(BUILD_DIR)/%.asm.o: %.asm
	@echo "ASM " $<
	@mkdir -p $(dir $@)
	@$(ASM) $< -f elf -o $@