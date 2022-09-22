CMD_PREFIX = aarch64-linux-gnu-
CC = $(CMD_PREFIX)gcc
LD = $(CMD_PREFIX)ld
OBJCPY = $(CMD_PREFIX)objcopy

ASM_FLAGS = -g

SRC_DIR = src
DES_DIR = build

.PHONY: build clean debug run

run: build
	qemu-system-aarch64 -M raspi3b -kernel $(DES_DIR)/kernel8.img -display none -d in_asm

debug: build
	qemu-system-aarch64 -M raspi3b -kernel $(DES_DIR)/kernel8.img -display none -d in_asm -s -S

build: clean $(DES_DIR)/kernel8.img

$(DES_DIR)/start.o: $(SRC_DIR)/start.s
	mkdir -p $(dir $@)
	$(CC) $(ASM_FLAGS) -c $(SRC_DIR)/start.s -o $(DES_DIR)/start.o

$(DES_DIR)/kernel8.img: $(DES_DIR)/start.o
	mkdir -p $(dir $@)
	$(LD) -T $(SRC_DIR)/linker.ld -o $(DES_DIR)/kernel8.elf $(DES_DIR)/start.o
	$(OBJCPY) -O binary $(DES_DIR)/kernel8.elf $(DES_DIR)/kernel8.img

clean:
	rm -rf $(DES_DIR) || true