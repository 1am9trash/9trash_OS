#include "mbox.h"
#include "uart.h"

// make a mailbox call. 
// returns 0 on failure, non-zero on success
int mbox_call(volatile uint32_t *mbox, uint8_t ch) {
    uint32_t r = (((uint32_t)((uint64_t)mbox) & ~0xF) | (ch & 0xF));

    // wait until we can write to the mailbox
    while (*MBOX_STATUS & MBOX_FULL)
        asm volatile("nop");

    // write the address of our message to the mailbox with channel identifier
    *MBOX_WRITE = r;
    // wait for the response
    while (1) {
        // is there a response
        while (*MBOX_STATUS & MBOX_EMPTY)
            asm volatile("nop");
        // is the response expected
        if (r == *MBOX_READ)
            // is the response valid
            return mbox[1] == MBOX_RESPONSE;
    }
    return 0;
}

void mbox_get_board_revision() {
    volatile uint32_t __attribute__((aligned(16))) mbox[7];
    mbox[0] = 7 * 4;                    // buffer size in bytes
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_GET_BOARD_REVISION;  // tag identifier
    mbox[3] = 4;                        // maximum of request and response value buffer's length
    mbox[4] = 0;                        // tag code
    mbox[5] = 0;                        // value buffer
    mbox[6] = MBOX_END_TAG;             // tag end
   
    if (mbox_call(mbox, MBOX_CH_PROP)) {      
        uart_puts("Board revision: ");
        uart_hex(mbox[5], 1);
        uart_puts("\n");
    } else
        uart_puts("Failed to get board revision.\n");
}

void mbox_get_arm_memory() {
    volatile uint32_t __attribute__((aligned(16))) mbox[8];
    mbox[0] = 8 * 4;                    // buffer size in bytes
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_GET_ARM_MEMORY;      // tag identifier
    mbox[3] = 8;                        // maximum of request and response value buffer's length
    mbox[4] = 0;                        // tag code
    mbox[5] = 0;                        // value buffer
    mbox[6] = 0;
    mbox[7] = MBOX_END_TAG;             // tag end
   
    if (mbox_call(mbox, MBOX_CH_PROP)) {      
        uart_puts("ARM memory base address: ");
        uart_hex(mbox[5], 1);
        uart_puts("\n");
        uart_puts("ARM memory size: ");
        uart_hex(mbox[6], 1);
        uart_puts("\n");
    } else
        uart_puts("Failed to get arm memory.\n");
}

void mbox_get_serial() {
    volatile uint32_t __attribute__((aligned(16))) mbox[8];
    mbox[0] = 8 * 4;                    // buffer size in bytes
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_GET_SERIAL;          // tag identifier
    mbox[3] = 8;                        // maximum of request and response value buffer's length
    mbox[4] = 0;                        // tag code
    mbox[5] = 0;                        // value buffer
    mbox[6] = 0;
    mbox[7] = MBOX_END_TAG;             // tag end

    if (mbox_call(mbox, MBOX_CH_PROP)) {      
        uart_puts("Serial number: ");
        uart_hex(mbox[6], 1);
        uart_hex(mbox[5], 0);
        uart_puts("\n");
    } else
        uart_puts("Failed to get serial.\n");
}