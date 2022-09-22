.section ".text.boot"

.global _start

_start:
  mrs     x1, mpidr_el1     // get cpu id
  and     x1, x1, #3        // [0:2] is enough
  cbz     x1, 2f            // if cpu id = 0, goto 2f
1:
  wfe
  b       1b
2:
  ldr     x1, =_start
  mov     sp, x1            // set stack pointer 

  ldr     x1, =__bss_start  // clear bss
  ldr     w2, =__bss_size
3:
  cbz     w2, 4f
  str     xzr, [x1], #8
  sub     w2, w2, #1
  cbnz    w2, 3b            // if w2 != 0, goto 3b
4:
  bl      main
  b       1b                // halt core when return