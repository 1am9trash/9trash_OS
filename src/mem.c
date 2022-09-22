#include "mem.h"
#include "uart.h"
#include "string.h"

void *simple_malloc(uint32_t size) {
    static char heap[HEAP_SIZE];
    static uint32_t head;

    if (head + size > HEAP_SIZE)
        return (char *)0;
    char *target = &heap[head];
    head += size;
    return target;
}

void test_simple_malloc() {
    uart_puts("Test simple_malloc():\n");

    uart_puts("\ns1 = malloc(13), s1 = \"Hello World!\"\n");
    char *s1 = (char *)simple_malloc(13);
    strcpy("Hello World", s1);
    uart_puts("s1 address: ");
    uart_hex((uint64_t)s1, 1);
    uart_puts("\n");
    uart_puts("s1 value: ");
    uart_puts(s1);
    uart_puts("\n");

    uart_puts("\ns2 = malloc(19), s2 = \"It's now or never.\"\n");
    char *s2 = (char *)simple_malloc(19);
    strcpy("It's now or never!", s2);
    uart_puts("s2 address: ");
    uart_hex((uint64_t)s2, 1);
    uart_puts("\n");
    uart_puts("s2 value: ");
    uart_puts(s2);
    uart_puts("\n");
}