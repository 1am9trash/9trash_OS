SECTIONS
{
  . = 0x80000;
  .text : { KEEP(*(.text.boot)) *(.text .text.*) }
  .rodata : { *(.rodata .rodata.*) }
  .data : { *(.data .data.*) }
  .bss ALIGN(16) (NOLOAD) :
  {
    __bss_start = .;
    *(.bss .bss.*)
    __bss_end = .;
  }
}
__bss_size = (__bss_end - __bss_start) >> 3;