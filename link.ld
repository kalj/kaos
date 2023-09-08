
ENTRY(start)

SECTIONS {
  . = 0x10000;
  .text : {
    *(.text.start)
    *(.text)
  }
  .data : { *(.data) }
  .rodata : { *(.rodata) }
  .bss : { *(.bss) }
}
