#include "uart.h"
#include "mbox.h"
#include "reboot.h"
#include "string.h"

void main() {
    uart_init();

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
        uart_gets(command, 64);

        if (strcmp(command, "help") == 0) {
            uart_puts("help\t: print this help memu\n");
            uart_puts("hello\t: print Hello World!\n");
            uart_puts("mailbox\t: print mailbox information\n");
            uart_puts("reboot\t: reboot the device\n");
        } else if (strcmp(command, "hello") == 0)
            uart_puts("Hello World!\n");
        else if (strcmp(command, "mailbox") == 0) {
            mbox_get_board_revision();
            mbox_get_arm_memory();
            mbox_get_serial();
        } else if (strcmp(command, "reboot") == 0) {
            uart_puts("Reboot in 10 tick.\n");
            reset(10);
        } else if (strcmp(command, "") != 0)
            uart_puts("Command is not found.\n");
    }
}