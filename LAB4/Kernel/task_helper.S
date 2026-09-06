.section ".text"
.global set_current
.global get_current
.global switch_to
.global enter_user

set_current:
  msr tpidr_el1, x0
  ret

get_current:
  mrs x0, tpidr_el1
  ret

switch_to:
  // Save current task context
  stp x19, x20, [x0, #0]
  stp x21, x22, [x0, #16]
  stp x23, x24, [x0, #32]
  stp x25, x26, [x0, #48]
  stp x27, x28, [x0, #64]
  stp x29, x30, [x0, #80]
  mov x9, sp
  str x9, [x0, #96]

  // Restore next task context
  ldp x19, x20, [x1, #0]
  ldp x21, x22, [x1, #16]
  ldp x23, x24, [x1, #32]
  ldp x25, x26, [x1, #48]
  ldp x27, x28, [x1, #64]
  ldp x29, x30, [x1, #80]
  ldr x9, [x1, #96]
  mov sp, x9

  ret

enter_user:
  mov x30, x0 // x0 = struct trapframe *tf

  // Restore User System State
  ldr x9, [x30, #248]
  msr sp_el0, x9
  ldr x9, [x30, #256]
  msr elr_el1, x9
  ldr x9, [x30, #264]
  msr spsr_el1, x9

  mov sp, x1 // x1 = kernel stack top

  // Restore User Registers
  ldp x0,  x1,  [x30, #0]
  ldp x2,  x3,  [x30, #16]
  ldp x4,  x5,  [x30, #32]
  ldp x6,  x7,  [x30, #48]
  ldp x8,  x9,  [x30, #64]
  ldp x10, x11, [x30, #80]
  ldp x12, x13, [x30, #96]
  ldp x14, x15, [x30, #112]
  ldp x16, x17, [x30, #128]
  ldp x18, x19, [x30, #144]
  ldp x20, x21, [x30, #160]
  ldp x22, x23, [x30, #176]
  ldp x24, x25, [x30, #192]
  ldp x26, x27, [x30, #208]
  ldp x28, x29, [x30, #224]
  ldr x30, [x30, #240]

  eret