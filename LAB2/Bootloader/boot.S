.section ".text.boot"
.global _start

_start:
  mrs x0, mpidr_el1
  and x0, x0, 3
  cbz x0, relocate

1:
  wfe
  b 1b

relocate:
  ldr x0, = 0x80000
  ldr x1, = 0x60000
  ldr x2, = __bss_start
  sub x2, x2, x1

copy_loop:
  ldr x3, [x0], #8
  str x3, [x1], #8
  subs x2, x2, #8
  b.gt copy_loop
  ldr x0, = relocate_start
  br x0  

relocate_start:
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