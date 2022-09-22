CMD_PREFIX = aarch64-linux-gnu-
CC = $(CMD_PREFIX)gcc
LD = $(CMD_PREFIX)ld
OBJCPY = $(CMD_PREFIX)objcopy

C_FLAGS = -Wall -g -ffreestanding -nostdinc -nostdlib -nostartfiles -I include
ASM_FLAGS = -g -I include

SRC_DIR = src
DES_DIR = build

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.s)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(DES_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.s=$(DES_DIR)/%_asm.o)

.PHONY: clean build debug run

run: build
	qemu-system-aarch64 -M raspi3b -kernel $(DES_DIR)/kernel8.img -initrd $(DES_DIR)/initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -serial null -serial stdio

debug: build
	qemu-system-aarch64 -M raspi3b -kernel $(DES_DIR)/kernel8.img -initrd $(DES_DIR)/initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -serial null -serial stdio -s -S

build: clean $(DES_DIR)/kernel8.img $(DES_DIR)/initramfs.cpio

$(DES_DIR)/initramfs.cpio:
	mkdir -p $(dir $@)
	cd test_file_system && find . | cpio -o -H newc > ../$(DES_DIR)/initramfs.cpio

$(DES_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) -c $< -o $@

$(DES_DIR)/%_asm.o: $(SRC_DIR)/%.s
	mkdir -p $(dir $@)
	$(CC) $(ASM_FLAGS) -c $< -o $@

$(DES_DIR)/kernel8.img: $(OBJ_FILES)
	mkdir -p $(dir $@)
	$(LD) -nostdlib -nostartfiles $(OBJ_FILES) -T $(SRC_DIR)/linker.ld -o $(DES_DIR)/kernel8.elf
	$(OBJCPY) -O binary $(DES_DIR)/kernel8.elf $(DES_DIR)/kernel8.img

clean:
	rm -rf $(DES_DIR) || true