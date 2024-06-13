#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "stdint.h"

uint32_t get_el();
void check_el(uint32_t el);

void enable_interrupt();
void disable_interrupt();

void default_exception_handler();
void sync_handler();
void irq_handler();

#endif