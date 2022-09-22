#ifndef _UART_H_
#define _UART_H_

#include "stdint.h"

void uart_init();
char uart_getc();
void uart_gets(char *s, uint32_t n);
void uart_putc(uint32_t c);
void uart_puts(char *s);
void uart_hex(uint32_t d);

#endif