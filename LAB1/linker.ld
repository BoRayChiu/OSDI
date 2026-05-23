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
}