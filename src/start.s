.section ".text.boot"

.global _start

_start:
  mrs x1, mpidr_el1         // get cpu id
  and x1, x1, #3            // [0:2] is enough
  cbz x1, 2f                // if cpu id = 0, goto 2f

1:
  wfe
  b 1b

2:
  ldr x1, =_start           // set stack pointer 
  mov sp, x1              
  ldr x1, =__bss_start      // clear bss
  ldr w2, =__bss_size

3:
  cbz w2, 4f
  str xzr, [x1], #8
  sub w2, w2, #1
  cbnz w2, 3b               // if w2 != 0, goto 3b

4:
  bl go_el1
  bl set_exception_vector_table
  bl core_timer_enable

  bl go_el0                 // make program run in el0
  bl main
  b 1b                      // halt core when return

go_el1:
  mov x0, (1 << 31)         // EL1 uses aarch64
  msr hcr_el2, x0
  mov x0, 0x3c5             // EL1h (SPSel = 1) with interrupt disabled
                            // (0b1111 << 6) | (0b0101 << 0)
  msr spsr_el2, x0
  adr x1, set_el1
  msr elr_el2, x1
  eret                      // return to EL1

set_el1:
  ldr x1, =0x60000          // set stack pointer 
  mov sp, x1
  ret

.global go_el0
.type go_el0, %function
go_el0:
  mov x0, 0x0
  msr spsr_el1, x0
  adr x1, set_el0
  msr elr_el1, x1
  eret                      // return to EL0

set_el0:
  ldr x1, =0x40000          // set stack pointer 
  mov sp, x1
  ret

// save general registers to stack
.macro save_all
  sub sp, sp, 32 * 8
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
.endm

// load general registers from stack
.macro load_all
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
  add sp, sp, 32 * 8
.endm

exception_handler_loop:
  b exception_handler_loop

exception_handler:
  save_all
  bl exception_entry
  load_all
  eret

exception_handler_lower_irq:
  save_all
  bl lower_irq_router
  load_all
  eret

.align 11                   // vector table should be aligned to 0x800
.global exception_vector_table
exception_vector_table:
  b exception_handler       // branch to a handler function
  .align 7                  // entry size is 0x80, .align will pad 0
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler_lower_irq
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

set_exception_vector_table:
  adr x0, exception_vector_table
  msr vbar_el1, x0
  ret

.equ CORE0_TIMER_IRQ_CTRL, 0x40000040

core_timer_enable:
  mov x0, 1
  msr cntp_ctl_el0, x0      // enable
  mrs x0, cntfrq_el0
  msr cntp_tval_el0, x0     // set expired time
  mov x0, 2
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str w0, [x1]              // unmask timer interrupt
  ret