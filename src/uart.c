#include "uart.h"
#include "gpio.h"
#include "aux.h"
#include "irq.h"
#include "exception.h"

struct Queue uart_write, uart_read;

// initial uart message queue
void queue_init(struct Queue *q, int size) {
    q->front = 0;
    q->rear = 0;
    q->size = size;
}

int queue_is_empty(struct Queue *q) {
    return q->front == q->rear;
}

int queue_is_full(struct Queue *q) {
    return q->front == (q->rear + 1) % q->size;
}

void queue_push(struct Queue *q, char c) {
    // drop if full
    if (queue_is_full(q))
        return; 
    q->buf[q->rear] = c;
    q->rear = (q->rear + 1) % q->size;
}

char queue_pop(struct Queue *q) {
    if (queue_is_empty(q)) 
        return '\0';
    char c = q->buf[q->front];
    q->front = (q->front + 1) % q->size;
    return c;
}

// uart handler for interrupt
void uart_handler() {
    // 2: can transmit to uart, 1: can receive from uart
    if (*AUX_MU_IIR & 2) {
        char c = queue_pop(&uart_write);
        *AUX_MU_IO = c;
        if (queue_is_empty(&uart_write))
            *AUX_MU_IER &= ~2;
    } else if (*AUX_MU_IIR & 4) {
        if (queue_is_full(&uart_read))
            return;
        char c = (char)(*AUX_MU_IO);
        queue_push(&uart_read, c);
    }
}

// initial mini UART and map to GPIO (may set in el1)
void uart_init() {
    *AUX_ENABLES |= 1;      // enable mini UART
    *AUX_MU_CNTL = 0;       // disable TX, RX during configuration
    *AUX_MU_IER = 1;        // enable interrupt
    *AUX_MU_LCR = 3;        // set the data size to 8 bit
    *AUX_MU_MCR = 0;        // don't need auto flow control
    *AUX_MU_BAUD = 270;     // set baud rate to 115200
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

    // setting for asynchronous read and write
    *IRQ_ENABLE_1 |= 1 << 29;
    enable_interrupt();
    queue_init(&uart_read, 1024);
    queue_init(&uart_write, 1024);
}

// send a character
void uart_putc(uint32_t c) { 
    // interrupt method
    int is_empty = queue_is_empty(&uart_write);
    queue_push(&uart_write, c); // push to write queue, drop if buffer full
    if (is_empty)
        *AUX_MU_IER |= 2;       // 2: can transmit to uart, 1: can receive from uart
}

// receive a character
char uart_getc() {
    // interrupt method
    while (queue_is_empty(&uart_read))
        asm volatile ("nop");
    char r = queue_pop(&uart_read);
    return r == '\r' ? '\n' : r;        // convert carrige return to newline
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

// receive a string
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
void uart_hex(uint32_t d, int prefix) {
    if (prefix)
        uart_puts("0x");
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

void uart_hex_long(uint64_t d, int prefix) {
    if ((d >> 32) != 0) {
        uart_hex(d >> 32, prefix);
        uart_hex(d << 32 >> 32, 0);
    } else
        uart_hex(d, prefix);
}