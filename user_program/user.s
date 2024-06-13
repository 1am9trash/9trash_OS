.section ".text"

.global _start

_start:
    mov x0, 0
    
1:
    add x0, x0, 1
    svc 1234
    cmp x0, 3
    blt 1b
    ret