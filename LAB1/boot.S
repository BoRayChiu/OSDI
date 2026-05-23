.section ".text"
.global _start

_start:
  mrs x0, mpidr_el1
  and x0, x0, 3
  cbz x0, 2f

1:
  wfe
  b 1b

2:
  ldr x0, = _stack_top
  mov sp, x0
  ldr x0, = __bss_start
  ldr x1, = __bss_end

3:
  cmp x0, x1
  b.eq 4f
  str xzr, [x0], #8 
  b 3b

4:
  bl main

5:
  wfe
  b 5b