#ifndef _AUX_H_
#define _AUX_H_

#include "stdint.h"
#include "mmio.h"

#define AUX_BASE        (MMIO_BASE + 0x215000)
#define AUX_ENABLES     ((volatile uint32_t *)(AUX_BASE + 0x04))
#define AUX_MU_IO       ((volatile uint32_t *)(AUX_BASE + 0x40))
#define AUX_MU_IER      ((volatile uint32_t *)(AUX_BASE + 0x44))
#define AUX_MU_IIR      ((volatile uint32_t *)(AUX_BASE + 0x48))
#define AUX_MU_LCR      ((volatile uint32_t *)(AUX_BASE + 0x4C))
#define AUX_MU_MCR      ((volatile uint32_t *)(AUX_BASE + 0x50))
#define AUX_MU_LSR      ((volatile uint32_t *)(AUX_BASE + 0x54))
#define AUX_MU_MSR      ((volatile uint32_t *)(AUX_BASE + 0x58))
#define AUX_MU_SCRATCH  ((volatile uint32_t *)(AUX_BASE + 0x5C))
#define AUX_MU_CNTL     ((volatile uint32_t *)(AUX_BASE + 0x60))
#define AUX_MU_STAT     ((volatile uint32_t *)(AUX_BASE + 0x64))
#define AUX_MU_BAUD     ((volatile uint32_t *)(AUX_BASE + 0x68))

#endif