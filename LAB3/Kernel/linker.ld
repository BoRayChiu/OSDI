SECTIONS
{
  . = 0x80000;
  _stack_top = .;
  .text : { *(.text) }
  .rodata : { *(.rodata) }
  .data : { *(.data) }
  .bss : {
    . = ALIGN(8);
    __bss_start = .;
    *(.bss)
    . = ALIGN(8);
    __bss_end = .;
  }
  . = ALIGN(16);
  _el0_stack_bottom = .;
  . += 0x4000;              /* 16 KB */
  _el0_stack_top = .;
}