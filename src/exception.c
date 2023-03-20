#include "exception.h"
#include "irq.h"
#include "uart.h"
#include "timer.h"

void exception_entry() {
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
}

void lower_irq_router() {
    uint32_t core0_intr_src = *CORE0_IRQ_SRC;
    if (core0_intr_src & (1 << 1))
        core_timer_handler();
    else
        uart_handler();
}