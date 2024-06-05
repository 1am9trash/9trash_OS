#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "stdint.h"

void go_el0(void);
void core_timer_handler(void);

void exception_entry();
void lower_irq_router();

#endif