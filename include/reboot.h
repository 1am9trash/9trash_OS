#ifndef _REBOOT_H_
#define _REBOOT_H_

#define PM_PASSWORD     0x5a000000
#define PM_RSTC         0x3F10001c
#define PM_WDOG         0x3F100024

#include "stdint.h"

void set(uint64_t addr, uint32_t value);
void reset(uint32_t tick);
void cancel_reset();

#endif