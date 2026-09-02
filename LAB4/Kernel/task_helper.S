.section ".text"
.global set_current
.global get_current

set_current:
  msr tpidr_el1, x0
  ret

get_current:
  mrs x0, tpidr_el1
  ret