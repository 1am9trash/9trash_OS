#ifndef _UART_H_
#define _UART_H_

#include "stdint.h"

#define QUEUE_MAX_SIZE 2048

// circular queue buffer
struct Queue {
    int front;
    int rear;
    int size;
    char buf[QUEUE_MAX_SIZE];
};

void queue_init(struct Queue *q, int size);
int queue_is_empty(struct Queue *q);
int queue_is_full(struct Queue *q);
void queue_push(struct Queue *q, char c);
char queue_pop(struct Queue *q);

void uart_handler();
void uart_init();
char uart_getc();
void uart_gets(char *s, uint32_t n);
void uart_putc(uint32_t c);
void uart_puts(char *s);
void uart_hex(uint32_t d, int prefix);
void uart_hex_long(uint64_t d, int prefix);

#endif