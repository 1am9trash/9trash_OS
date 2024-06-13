#include "exception.h"
#include "irq.h"
#include "uart.h"

// may not use in el0
uint32_t get_el() {
    uint32_t el;
    asm volatile("mrs %0, CurrentEl" : "=r" (el));
    return (el >> 2);
}

void check_el(uint32_t el) {
    uint32_t cur_el = get_el();
    if (cur_el != el) {
        uart_puts("check el problem: current el = ");
        uart_hex(cur_el, 1);
        uart_puts("\n");
    }
}

void enable_interrupt() {
    asm volatile("msr DAIFClr, 0xf");
}

void disable_interrupt() {
    asm volatile("msr DAIFSet, 0xf");
}

void default_exception_handler() {
    // do nothing
}

void sync_handler() {
    disable_interrupt();
    check_el(1);

    uint64_t spsrel1, elrel1, esrel1;

    asm volatile ("mrs %0, SPSR_EL1" : "=r" (spsrel1));
    uart_puts("SPSR_EL1: ");
    uart_hex_long(spsrel1, 1);
    uart_puts("\n");

    asm volatile ("mrs %0, ELR_EL1" : "=r" (elrel1));
    uart_puts(" ELR_EL1: ");
    uart_hex_long(elrel1, 1);
    uart_puts("\n");

    asm volatile ("mrs %0, ESR_EL1" : "=r" (esrel1));
    uart_puts(" ESR_EL1: ");
    uart_hex_long(esrel1, 1);
    uart_puts("\n");

    enable_interrupt();
}

void irq_handler() {
    disable_interrupt();
    check_el(1);

    // check whether the interrupt is triggered by mini UART or not
    if (*IRQ_PENDING_1 & (1 << 29))
        uart_handler();

    enable_interrupt();
}