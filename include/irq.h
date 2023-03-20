#ifndef _IRQ_H_
#define _IRQ_H_

#include "stdint.h"
#include "mmio.h"

#define IRQ_BASE                (MMIO_BASE + 0xb000)

#define IRQ_BASIC_PENDING       ((unsigned int*)(IRQ_BASE + 0x200))
#define IRQ_ENABLE_1            ((unsigned int*)(IRQ_BASE + 0x210))
#define IRQ_ENABLE_2            ((unsigned int*)(IRQ_BASE + 0x214))
#define IRQ_ENABLE_BASIC        ((unsigned int*)(IRQ_BASE + 0x218))

#define CORE0_IRQ_SRC ((unsigned int*)(0x40000060))
#define CORE1_IRQ_SRC ((unsigned int*)(0x40000064))
#define CORE2_IRQ_SRC ((unsigned int*)(0x40000068))
#define CORE3_IRQ_SRC ((unsigned int*)(0x4000006c))

#endif