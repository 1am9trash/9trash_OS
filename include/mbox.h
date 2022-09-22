#ifndef _MBOX_H_
#define _MBOX_H_

#include "stdint.h"
#include "gpio.h"

#define MBOX_BASE       (MMIO_BASE + 0xb880)
#define MBOX_READ       ((volatile uint32_t *)(MBOX_BASE + 0x00))
#define MBOX_POLL       ((volatile uint32_t *)(MBOX_BASE + 0x10))
#define MBOX_SENDER     ((volatile uint32_t *)(MBOX_BASE + 0x14))
#define MBOX_STATUS     ((volatile uint32_t *)(MBOX_BASE + 0x18))
#define MBOX_CONFIG     ((volatile uint32_t *)(MBOX_BASE + 0x1C))
#define MBOX_WRITE      ((volatile uint32_t *)(MBOX_BASE + 0x20))

#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000      // 31 bit in MBOX_STATUS
#define MBOX_EMPTY      0x40000000      // 30 bit in MBOX_STATUS

#define MBOX_REQUEST    0

// channels
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8

// tags
#define MBOX_GET_BOARD_REVISION     0x010002
#define MBOX_GET_SERIAL             0x010004
#define MBOX_GET_ARM_MEMORY         0x010005
#define MBOX_END_TAG                0

int mbox_call(volatile uint32_t *mbox, uint8_t ch);
void mbox_get_board_revision();
void mbox_get_arm_memory();
void mbox_get_serial();

#endif