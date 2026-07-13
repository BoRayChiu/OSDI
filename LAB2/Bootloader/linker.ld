SECTIONS
{
  . = 0x60000;
  _stack_top = .;
  .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
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