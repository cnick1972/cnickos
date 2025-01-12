CC			:= clang
LD 			:= ld.lld
ASM 		:= nasm

CCFLAGS := -std=c99 -c -ffreestanding -target i686-none-elf -I. -I../src/libs
LINKFLAGS := -L $(shell pwd)/../../../ -nostdlib --oformat binary

BUILD_DIR := ../../../build/bootloader/stage2

STAGE2DIR	:= .

STAGE2_SRC :=  $(shell find $(STAGE2DIR) -name '*.asm' -or -name '*.c')
STAGE2_OBJS := $(STAGE2_SRC:%=$(BUILD_DIR)/%.o)

all: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/stage2.bin: $(STAGE2_OBJS)
	@echo "LD  " $@
	@ld.lld -T $(STAGE2DIR)/linker.ld $(LINKFLAGS) -Map=$(BUILD_DIR)/stage2.map -o $@ $(STAGE2_OBJS) -lgcc


$(BUILD_DIR)/%.c.o: %.c
	@echo "CC  " $<
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -o $@ $<

$(BUILD_DIR)/%.asm.o: %.asm
	@echo "ASM " $<
	@mkdir -p $(dir $@)
	@$(ASM) $< -f elf -o $@