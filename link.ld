
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
  /* .vectors : { KEEP(*(.vectors)) } > pflash */
  /* .text    : { *(.text*) } > pflash */
  /* .rodata  : { *(.rodata*) } > pflash */

  /* .data : { */
  /*   _DATA_START = .; */
  /*   /\* *(.first_data) *\/ */
  /*   *(.data SORT(.data.*)) */
  /*   _DATA_END = .; */
  /*  } > sram AT > pflash */
  /*  /\* } > sram AT > dflash *\/ */

  /* _DATA_LOAD_ADDRESS = LOADADDR(.data); */

  /* .bss : { */
  /*   _BSS_START = .; */
  /*   *(.bss SORT(.bss.*) COMMON) */
  /*   _BSS_END = .; */
  /*  } > sram */
}
