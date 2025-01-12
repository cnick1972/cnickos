CC			:= clang
LD 			:= ld.lld
ASM 		:= nasm

CCFLAGS := -std=c99 -c -ffreestanding -target i686-none-elf -I$(shell pwd)/arch/x86/include/ -I. -I../src/libs
LINKFLAGS := -L $(shell pwd)/../../ -nostdlib --oformat binary

BUILD_DIR := ../../build/kernel

CRTI_OBJ := ./crt/crti.o
CRTN_OBJ := ./crt/crtn.o



KERNELDIR := .
KERNEL_SRC := $(shell find $(KERNELDIR) -name '*.asm' -or -name '*.c')
KERNEL_OBJS := $(KERNEL_SRC:%=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(KERNEL_OBJS)
	@echo "LD  " $@
	@ld.lld -T $(KERNELDIR)/linker.ld $(LINKFLAGS) -Map=$(BUILD_DIR)/kernel.map -o $@ $(CRTI_OBJ) $(KERNEL_OBJS) $(CRTN_OBJ) -lgcc

$(BUILD_DIR)/%.c.o: %.c
	@echo "CC  " $<
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -o $@ $<

$(BUILD_DIR)/%.asm.o: %.asm
	@echo "ASM " $<
	@mkdir -p $(dir $@)
	@$(ASM) $< -I$(shell pwd)/arch/x86/ -f elf -o $@