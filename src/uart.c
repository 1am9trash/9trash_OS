#include "uart.h"
#include "gpio.h"
#include "aux.h"

// initial mini UART and map to GPIO
void uart_init() {
    *AUX_ENABLES |= 1;      // Enable mini UART
    *AUX_MU_CNTL = 0;       // Disable TX, RX during configuration
    *AUX_MU_IER = 0;        // Disable interrupt
    *AUX_MU_LCR = 3;        // Set the data size to 8 bit
    *AUX_MU_MCR = 0;        // Don't need auto flow control
    *AUX_MU_BAUD = 270;     // Set baud rate to 115200
    *AUX_MU_IIR = 0xc6;     // disable interrupts

    uint32_t r;
    r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15)); // gpio14, gpio15
    r |= (2 << 12) | (2 << 15);    // alt5 for mini UART
    *GPFSEL1 = r;

    *GPPUD = 0;
    r = 150;
    while (r--)
        asm volatile("nop");

    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150;
    while (r--)
        asm volatile("nop");
    
    *GPPUDCLK0 = 0;         
    *AUX_MU_CNTL = 3;       // enable Tx, Rx
}

// send a character
void uart_putc(uint32_t c) {
    // wait until we can send
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x20));
    *AUX_MU_IO = c;         // write the character to the buffer
}

// receive a character
char uart_getc() {
    // wait until something is in the buffer
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x01));
    char r = (char)(*AUX_MU_IO);
    return r == '\r' ? '\n' : r;    // convert carrige return to newline
}

// display a string
void uart_puts(char *s) {
    while (*s) {
        // convert newline to carrige return + newline
        if (*s == '\n')
            uart_putc('\r');
        uart_putc(*s++);
    }
}

// receive a strung
void uart_gets(char *s, uint32_t n) {
    char c;
    int cnt = 0;
    while (1) {
        c = uart_getc();
        if (c == '\n' || cnt == n - 1) {
            uart_puts("\n");
            s[cnt] = '\0';
            break;
        } else
            uart_putc(c);

        s[cnt++] = c;
    }
}

// display a binary value in hexadecimal
void uart_hex(uint32_t d) {
    uint32_t n;
    int c;
    for (c = 28; c >= 0; c -= 4) {
        // get highest tetrad
        n = (d >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += n > 9 ? 0x37 : 0x30;
        uart_putc(n);
    }
}