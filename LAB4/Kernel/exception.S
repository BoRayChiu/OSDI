.section ".text"
.align 11
.global exception_table_el2
.global exception_table_el1
.global enable_irq_el1
.global disable_irq_el1
.global irq_enable_nested
.global irq_disable_nested
.global switch_to_el1
.global switch_to_el0
.extern synchronous_exception_handler
.extern el1_entry
.extern shell
.extern el0_synchronous_exception_handler

exception_table_el2:
  // 0x000
  b default_exception_handler
  .align 7

  // 0x080
  b default_exception_handler
  .align 7

  // 0x100
  b default_exception_handler
  .align 7

  // 0x180
  b default_exception_handler
  .align 7

  // 0x200
  // Current EL, SP_ELx, Synchronous Exceptions
  b el2_synchronous_handler
  .align 7

  // 0x280
  // Current EL, SP_ELx, IRQ
  b el2_irq_handler
  .align 7

  // 0x300
  b default_exception_handler
  .align 7

  // 0x380
  b default_exception_handler
  .align 7

  // 0x400
  b default_exception_handler
  .align 7

  // 0x480
  b default_exception_handler
  .align 7

  // 0x500
  b default_exception_handler
  .align 7

  // 0x580
  b default_exception_handler
  .align 7

  // 0x600
  b default_exception_handler
  .align 7

  // 0x680
  b default_exception_handler
  .align 7

  // 0x700
  b default_exception_handler
  .align 7

  // 0x780
  b default_exception_handler
  .align 7

exception_table_el1:
  // 0x000
  b default_exception_handler
  .align 7

  // 0x080
  b default_exception_handler
  .align 7

  // 0x100
  b default_exception_handler
  .align 7

  // 0x180
  b default_exception_handler
  .align 7

  // 0x200
  // Current EL, SP_ELx, Synchronous Exceptions
  b el1_synchronous_handler
  .align 7

  // 0x280
  // Current EL, SP_ELx, IRQ
  b el1_irq_handler
  .align 7

  // 0x300
  b default_exception_handler
  .align 7

  // 0x380
  b default_exception_handler
  .align 7

  // 0x400
  // Lower EL, SP_EL0, Synchronous Exceptions
  b el0_synchronous_handler
  .align 7

  // 0x480
  // Lower EL, SP_EL0, IRQ
  b el1_irq_handler
  .align 7

  // 0x500
  b default_exception_handler
  .align 7

  // 0x580
  b default_exception_handler
  .align 7

  // 0x600
  b default_exception_handler
  .align 7

  // 0x680
  b default_exception_handler
  .align 7

  // 0x700
  b default_exception_handler
  .align 7

  // 0x780
  b default_exception_handler
  .align 7
  
el2_synchronous_handler:
  // Allocate 256 bytes on stack
  sub sp, sp, #256
  stp x0,  x1, [sp, #0]
  stp x2,  x3, [sp, #16]
  stp x4,  x5,  [sp, #32]
  stp x6,  x7,  [sp, #48]
  stp x8,  x9,  [sp, #64]
  stp x10, x11, [sp, #80]
  stp x12, x13, [sp, #96]
  stp x14, x15, [sp, #112]
  stp x16, x17, [sp, #128]
  stp x18, x19, [sp, #144]
  stp x20, x21, [sp, #160]
  stp x22, x23, [sp, #176]
  stp x24, x25, [sp, #192]
  stp x26, x27, [sp, #208]
  stp x28, x29, [sp, #224]
  str x30, [sp, #240]
  mrs x0, elr_el2
  mrs x1, esr_el2
  bl synchronous_exception_handler
  // Restore x30 ~ x0
  ldr x30, [sp, #240]
  ldp x28, x29, [sp, #224]
  ldp x26, x27, [sp, #208]
  ldp x24, x25, [sp, #192]
  ldp x22, x23, [sp, #176]
  ldp x20, x21, [sp, #160]
  ldp x18, x19, [sp, #144]
  ldp x16, x17, [sp, #128]
  ldp x14, x15, [sp, #112]
  ldp x12, x13, [sp, #96]
  ldp x10, x11, [sp, #80]
  ldp x8,  x9,  [sp, #64]
  ldp x6,  x7,  [sp, #48]
  ldp x4,  x5,  [sp, #32]
  ldp x2,  x3,  [sp, #16]
  ldp x0,  x1,  [sp, #0]
  add sp, sp, #256
  eret

el1_synchronous_handler:
  // Allocate 256 bytes on stack
  sub sp, sp, #256
  stp x0,  x1, [sp, #0]
  stp x2,  x3, [sp, #16]
  stp x4,  x5,  [sp, #32]
  stp x6,  x7,  [sp, #48]
  stp x8,  x9,  [sp, #64]
  stp x10, x11, [sp, #80]
  stp x12, x13, [sp, #96]
  stp x14, x15, [sp, #112]
  stp x16, x17, [sp, #128]
  stp x18, x19, [sp, #144]
  stp x20, x21, [sp, #160]
  stp x22, x23, [sp, #176]
  stp x24, x25, [sp, #192]
  stp x26, x27, [sp, #208]
  stp x28, x29, [sp, #224]
  str x30, [sp, #240]
  mrs x0, elr_el1
  mrs x1, esr_el1
  bl synchronous_exception_handler
  // Restore x30 ~ x0
  ldr x30, [sp, #240]
  ldp x28, x29, [sp, #224]
  ldp x26, x27, [sp, #208]
  ldp x24, x25, [sp, #192]
  ldp x22, x23, [sp, #176]
  ldp x20, x21, [sp, #160]
  ldp x18, x19, [sp, #144]
  ldp x16, x17, [sp, #128]
  ldp x14, x15, [sp, #112]
  ldp x12, x13, [sp, #96]
  ldp x10, x11, [sp, #80]
  ldp x8,  x9,  [sp, #64]
  ldp x6,  x7,  [sp, #48]
  ldp x4,  x5,  [sp, #32]
  ldp x2,  x3,  [sp, #16]
  ldp x0,  x1,  [sp, #0]
  add sp, sp, #256
  eret

el0_synchronous_handler:
  // Allocate 272 bytes on stack
  sub sp, sp, #272
  stp x0,  x1, [sp, #0]
  stp x2,  x3, [sp, #16]
  stp x4,  x5,  [sp, #32]
  stp x6,  x7,  [sp, #48]
  stp x8,  x9,  [sp, #64]
  stp x10, x11, [sp, #80]
  stp x12, x13, [sp, #96]
  stp x14, x15, [sp, #112]
  stp x16, x17, [sp, #128]
  stp x18, x19, [sp, #144]
  stp x20, x21, [sp, #160]
  stp x22, x23, [sp, #176]
  stp x24, x25, [sp, #192]
  stp x26, x27, [sp, #208]
  stp x28, x29, [sp, #224]
  str x30, [sp, #240]
  // Save ELR_EL1
  mrs x2, elr_el1
  str x2, [sp, #248]
  // Save SPSR_EL1
  mrs x2, spsr_el1
  str x2, [sp, #256]
  // x0 = ESR_EL1
  mrs x0, esr_el1
  bl el0_synchronous_exception_handler
  // Restore SPSR_EL1
  ldr x2, [sp, #256]
  msr spsr_el1, x2
  // Restore ELR_EL1
  ldr x2, [sp, #248]
  msr elr_el1, x2
  // Restore x30 ~ x0
  ldr x30, [sp, #240]
  ldp x28, x29, [sp, #224]
  ldp x26, x27, [sp, #208]
  ldp x24, x25, [sp, #192]
  ldp x22, x23, [sp, #176]
  ldp x20, x21, [sp, #160]
  ldp x18, x19, [sp, #144]
  ldp x16, x17, [sp, #128]
  ldp x14, x15, [sp, #112]
  ldp x12, x13, [sp, #96]
  ldp x10, x11, [sp, #80]
  ldp x8,  x9,  [sp, #64]
  ldp x6,  x7,  [sp, #48]
  ldp x4,  x5,  [sp, #32]
  ldp x2,  x3,  [sp, #16]
  ldp x0,  x1,  [sp, #0]
  add sp, sp, #272
  eret

el2_irq_handler:
  // Allocate 272 bytes on stack
  sub sp, sp, #272
  stp x0,  x1, [sp, #0]
  stp x2,  x3, [sp, #16]
  stp x4,  x5,  [sp, #32]
  stp x6,  x7,  [sp, #48]
  stp x8,  x9,  [sp, #64]
  stp x10, x11, [sp, #80]
  stp x12, x13, [sp, #96]
  stp x14, x15, [sp, #112]
  stp x16, x17, [sp, #128]
  stp x18, x19, [sp, #144]
  stp x20, x21, [sp, #160]
  stp x22, x23, [sp, #176]
  stp x24, x25, [sp, #192]
  stp x26, x27, [sp, #208]
  stp x28, x29, [sp, #224]
  str x30, [sp, #240]

  // Store ELR_EL2
  mrs x0, elr_el2
  str x0, [sp, #248]

  // Store SPSR_EL2
  mrs x0, spsr_el2
  str x0, [sp, #256]

  bl irq_exception_handler

  // Restore SPSR_EL2
  ldr x0, [sp, #256]
  msr spsr_el2, x0

  // Restore ELR_EL2
  ldr x0, [sp, #248]
  msr elr_el2, x0

  // Restore x30 ~ x0
  ldr x30, [sp, #240]
  ldp x28, x29, [sp, #224]
  ldp x26, x27, [sp, #208]
  ldp x24, x25, [sp, #192]
  ldp x22, x23, [sp, #176]
  ldp x20, x21, [sp, #160]
  ldp x18, x19, [sp, #144]
  ldp x16, x17, [sp, #128]
  ldp x14, x15, [sp, #112]
  ldp x12, x13, [sp, #96]
  ldp x10, x11, [sp, #80]
  ldp x8,  x9,  [sp, #64]
  ldp x6,  x7,  [sp, #48]
  ldp x4,  x5,  [sp, #32]
  ldp x2,  x3,  [sp, #16]
  ldp x0,  x1,  [sp, #0]
  add sp, sp, #272
  eret

el1_irq_handler:
  // Allocate 272 bytes on stack
  sub sp, sp, #272
  stp x0,  x1, [sp, #0]
  stp x2,  x3, [sp, #16]
  stp x4,  x5,  [sp, #32]
  stp x6,  x7,  [sp, #48]
  stp x8,  x9,  [sp, #64]
  stp x10, x11, [sp, #80]
  stp x12, x13, [sp, #96]
  stp x14, x15, [sp, #112]
  stp x16, x17, [sp, #128]
  stp x18, x19, [sp, #144]
  stp x20, x21, [sp, #160]
  stp x22, x23, [sp, #176]
  stp x24, x25, [sp, #192]
  stp x26, x27, [sp, #208]
  stp x28, x29, [sp, #224]
  str x30, [sp, #240]

  // Store ELR_EL1
  mrs x0, elr_el1
  str x0, [sp, #248]

  // Store SPSR_EL1
  mrs x0, spsr_el1
  str x0, [sp, #256]

  bl irq_exception_handler

  // Restore SPSR_EL1
  ldr x0, [sp, #256]
  msr spsr_el1, x0

  // Restore ELR_EL1
  ldr x0, [sp, #248]
  msr elr_el1, x0

  // Restore x30 ~ x0
  ldr x30, [sp, #240]
  ldp x28, x29, [sp, #224]
  ldp x26, x27, [sp, #208]
  ldp x24, x25, [sp, #192]
  ldp x22, x23, [sp, #176]
  ldp x20, x21, [sp, #160]
  ldp x18, x19, [sp, #144]
  ldp x16, x17, [sp, #128]
  ldp x14, x15, [sp, #112]
  ldp x12, x13, [sp, #96]
  ldp x10, x11, [sp, #80]
  ldp x8,  x9,  [sp, #64]
  ldp x6,  x7,  [sp, #48]
  ldp x4,  x5,  [sp, #32]
  ldp x2,  x3,  [sp, #16]
  ldp x0,  x1,  [sp, #0]
  add sp, sp, #272
  eret

enable_irq_el1:
  // PSTATE.I = 0
  msr daifclr, #2
  ret

disable_irq_el1:
  // PSTATE.I = 1
  msr daifset, #2
  ret

irq_enable_nested:
  msr daifclr, #2
  ret

irq_disable_nested:
  msr daifset, #2
  ret

default_exception_handler:
  1:
    b 1b

switch_to_el1:
  // EL1 stack
  ldr x0, =_stack_top
  msr sp_el1, x0
  // HCR_EL2
  mrs x0, hcr_el2
  orr x0, x0, #(1 << 31) // RW = 1
  bic x0, x0, #(1 << 4) // IMO = 0
  msr hcr_el2, x0
  isb
  // Return State
  mov x0, #0x3c5
  msr spsr_el2, x0
  // Return Address
  ldr x0, =el1_entry
  msr elr_el2, x0
  isb
  
  eret

switch_to_el0:
  // EL0 stack
  ldr x0, =_el0_stack_top
  msr sp_el0, x0
  // Return State
  mov x0, #0
  msr spsr_el1, x0
  // Return Address
  ldr x0, =shell
  msr elr_el1, x0
  isb
  
  eret