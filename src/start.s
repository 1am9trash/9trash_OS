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