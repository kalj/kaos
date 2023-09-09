
ENTRY(entry)

SECTIONS {
  . = 0x500;
  .entry : { *(.entry) }
  .text : { *(.text) }
  .data : { *(.data) }
  .rodata : { *(.rodata) }
  .bss : { *(.bss) }
}
