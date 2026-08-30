.extern exception_table_el2
.extern exception_table_el1
.extern switch_to_el1

.section ".text"
.global _start
.global el1_entry

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
  ldr x0, =exception_table_el2
  msr vbar_el2, x0
  isb

5:
  bl switch_to_el1

el1_entry:
  ldr x0, =exception_table_el1
  msr vbar_el1, x0
  isb
  bl main

6:
  wfe
  b 6b
