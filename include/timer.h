#ifndef _TIMER_H_
#define _TIMER_H_

#include "stdint.h"

// relative: cntp_tval_el0
// absoulute: cntp_cval_el0

extern uint32_t print_core_timer_info;

struct TimerEvent {
    void (*callback)(void);
    uint64_t tval;
    struct TimerEvent *next;
};

extern struct TimerEvent *timer_queue;

void core_timer_handler();
void callback_timer();
void add_callback_timer(void *callback, uint64_t timeout);
void print_callback_timer_queue();
void set_timer_callback(void *callback);
void set_relative_timeout(uint64_t timeout);
void set_absolute_timeout(uint64_t timeout);
uint64_t get_absolute_time(uint64_t offset);
void print_routine_core_timer();

#endif