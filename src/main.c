#include "uart.h"
#include "mbox.h"
#include "reboot.h"
#include "string.h"
#include "cpio.h"
#include "utils.h"
#include "exception.h"

void shell() {
    uart_puts("\n");
    uart_puts("   ____  __                  __   \n");
    uart_puts("  / __ \\/ /__________ ______/ /_  \n");
    uart_puts(" / /_/ / __/ ___/ __ `/ ___/ __ \\ \n");
    uart_puts(" \\__, / /_/ /  / /_/ (__  ) / / / \n");
    uart_puts("/____/\\__/_/   \\__,_/____/_/ /_/  \n");
    uart_puts("\n[Welcome to the simple os shell]\n");

    while (1) {
        uart_puts("\r# ");
        char command[64];
        uart_gets(command, sizeof(command));

        if (strcmp(command, "help") == 0) {
            uart_puts("help\t\t: print this help menu\n");
            uart_puts("load\t\t: load user program and run in el0\n");
            uart_puts("info\t\t: print machine information from mailbox\n");
            uart_puts("reboot\t\t: reboot the device\n");
            uart_puts("ls\t\t: list all files in the initramfs\n");
            uart_puts("cat\t\t: print the content of a file\n");
        } else if (strcmp(command, "load") == 0) {
            if (cpio_get_file_address("user.img") == (void *)0)
                uart_puts("file is not found.\n");
            else {
                void *address = cpio_get_file_address("user.img");
                uart_puts("load user program and run in el0.\n");
                uart_puts("user program address: ");
                uart_hex_long((uint64_t)address, 1);
                uart_puts("\n\n");
                void (*load_user_program)(void) = (void (*)())address;
                load_user_program();
            }
        } else if (strcmp(command, "info") == 0) {
            mbox_get_board_revision();
            mbox_get_arm_memory();
            mbox_get_serial();
        } else if (strcmp(command, "reboot") == 0) {
            uart_puts("Reboot in 10 tick.\n");
            reset(10);
        } else if (strcmp(command, "ls") == 0) {
            ls();
        } else if (strcmp(command, "cat") == 0) {
            cat();
        } else if (strcmp(command, "") != 0) {
            uart_puts("Command is not found.\n");
        }
    }
}

void main() {
    shell();
}