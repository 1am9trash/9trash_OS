#include "reboot.h"

void set(uint64_t addr, uint32_t value) {
    volatile uint32_t *point = (uint32_t *)addr;
    *point = value;
}

void reset(uint32_t tick) {         // reboot after watchdog timer expire
    set(PM_RSTC, PM_PASSWORD | 0x20);   // full reset
    set(PM_WDOG, PM_PASSWORD | tick);   // number of watchdog tick
}

void cancel_reset() {
    set(PM_RSTC, PM_PASSWORD | 0);  // full reset
    set(PM_WDOG, PM_PASSWORD | 0);  // number of watchdog tick
}