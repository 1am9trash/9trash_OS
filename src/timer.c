#include "timer.h"
#include "uart.h"

extern int __heap_start;
static char *ptr = (char *)&__heap_start;

uint32_t print_core_timer_info = 0;     // don't display core timer info in default
struct TimerEvent *timer_queue = 0;

void core_timer_handler() {
    callback_timer();
    add_callback_timer(print_routine_core_timer, 2);
}

// find events which are needed to be called
void callback_timer() {
    struct TimerEvent *cur = timer_queue;
    uint64_t cur_time = get_absolute_time(0);
    while (cur != 0) {
        if (cur->tval > cur_time)
            break;
        cur->callback();
        cur = cur->next;
    }
    if (cur != 0) {
        timer_queue = cur;
        set_absolute_timeout(cur->tval);
    } else
        timer_queue = 0;
}

void add_callback_timer(void *callback, uint64_t timeout) {
    struct TimerEvent *event = (struct TimerEvent *)ptr;
    ptr += sizeof(struct TimerEvent);

    event->tval = get_absolute_time(timeout);
    event->callback = callback;
    event->next = 0;

    if (timer_queue == 0) {
        timer_queue = event;
        set_absolute_timeout(event->tval);
    } else {
        struct TimerEvent *cur = timer_queue;
        while (cur != 0) {
            if (event->tval > cur->tval) {
                event->next = cur->next;
                cur->next = event;
                break;
            }
            if (cur->next == 0) {
                cur->next = event;
                break;
            }
            cur = cur->next;
        }
    }
}

void print_callback_timer_queue() {
    struct TimerEvent *cur = timer_queue;
    while (1) {
        uart_hex_long(cur->tval, 1);
        uart_puts("\n");
        if (cur->next == 0)
            break;
        cur = cur->next;
    }
}

void set_relative_timeout(uint64_t timeout) {  
    asm volatile(
        "mrs x1, cntfrq_el0\n\t"
        "mul x1, x1, %0\n\t"
        "msr cntp_tval_el0, x1\n\t"
        ::"r"(timeout)
    );
}

void set_absolute_timeout(uint64_t timeout) {
    asm volatile("msr cntp_cval_el0, %0":"=r"(timeout));
}

uint64_t get_absolute_time(uint64_t offset) {
    uint64_t cntpct_el0, cntfrq_el0;
    asm volatile("mrs %0, cntfrq_el0":"=r"(cntfrq_el0));
    asm volatile("mrs %0, cntpct_el0":"=r"(cntpct_el0));
    return cntpct_el0 + cntfrq_el0 * offset;
}

void print_routine_core_timer() {
    if (print_core_timer_info) {
        uint64_t cntpct_el0, cntfrq_el0;
        asm volatile("mrs %0, cntfrq_el0":"=r"(cntfrq_el0));
        asm volatile("mrs %0, cntpct_el0":"=r"(cntpct_el0));

        uart_puts("Core timer: ");
        uart_hex_long(cntpct_el0 / cntfrq_el0, 1);
        uart_puts("\n");
    }
}