CMD_PREFIX = aarch64-linux-gnu-
LD = $(CMD_PREFIX)ld
OBJCPY = $(CMD_PREFIX)objcopy

CC = $(CMD_PREFIX)gcc

ASM_FLAGS = -g -I include

DES_DIR = ../test_file_system

.PHONY: clean build

build: clean linker.ld user.s
	mkdir -p $(DES_DIR)
	$(CC) $(ASM_FLAGS) -c user.s -o $(DES_DIR)/user_asm.o
	$(LD) -nostdlib -nostartfiles $(DES_DIR)/user_asm.o -T linker.ld -o $(DES_DIR)/user.elf
	$(OBJCPY) -O binary $(DES_DIR)/user.elf $(DES_DIR)/user.img

clean:
	rm -rf $(DES_DIR)/user*
