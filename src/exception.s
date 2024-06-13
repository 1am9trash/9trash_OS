#include "exception_table.h"

// save general registers to stack
.macro save_all
  sub sp, sp, 16 * 17

  stp x0, x1, [sp ,16 * 0]
  stp x2, x3, [sp ,16 * 1]
  stp x4, x5, [sp ,16 * 2]
  stp x6, x7, [sp ,16 * 3]
  stp x8, x9, [sp ,16 * 4]
  stp x10, x11, [sp ,16 * 5]
  stp x12, x13, [sp ,16 * 6]
  stp x14, x15, [sp ,16 * 7]
  stp x16, x17, [sp ,16 * 8]
  stp x18, x19, [sp ,16 * 9]
  stp x20, x21, [sp ,16 * 10]
  stp x22, x23, [sp ,16 * 11]
  stp x24, x25, [sp ,16 * 12]
  stp x26, x27, [sp ,16 * 13]
  stp x28, x29, [sp ,16 * 14]
  str x30, [sp, 16 * 15]

  mrs x22, elr_el1
  mrs x23, spsr_el1
  stp x22, x23, [sp, #16 * 16]
.endm

// load general registers from stack
.macro load_all
  ldp x22, x23, [sp, #16 * 16]
  msr elr_el1, x22
  msr spsr_el1, x23
  
  ldp x0, x1, [sp ,16 * 0]
  ldp x2, x3, [sp ,16 * 1]
  ldp x4, x5, [sp ,16 * 2]
  ldp x6, x7, [sp ,16 * 3]
  ldp x8, x9, [sp ,16 * 4]
  ldp x10, x11, [sp ,16 * 5]
  ldp x12, x13, [sp ,16 * 6]
  ldp x14, x15, [sp ,16 * 7]
  ldp x16, x17, [sp ,16 * 8]
  ldp x18, x19, [sp ,16 * 9]
  ldp x20, x21, [sp ,16 * 10]
  ldp x22, x23, [sp ,16 * 11]
  ldp x24, x25, [sp ,16 * 12]
  ldp x26, x27, [sp ,16 * 13]
  ldp x28, x29, [sp ,16 * 14]
  ldr x30, [sp, 16 * 15]

  add sp, sp, 16 * 17
.endm

exception_handler:
  save_all
  bl default_exception_handler
  load_all
  eret

exception_handler_sync:
  save_all
  bl sync_handler
  load_all
  eret

exception_handler_irq:
  save_all
  bl irq_handler
  load_all
  eret

.align 11                   // vector table should be aligned to 0x800
.global exception_vector_table
exception_vector_table:
  // el1t
  b exception_handler       // branch to a handler function
  .align 7                  // entry size is 0x80, .align will pad 0
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  // el1h
  b exception_handler_sync
  .align 7
  b exception_handler_irq
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  // 64-bit el0
  b exception_handler_sync
  .align 7
  b exception_handler_irq
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  // 32-bit el0
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

.global set_exception_vector_table
set_exception_vector_table:
  adr x0, exception_vector_table
  msr vbar_el1, x0
  ret