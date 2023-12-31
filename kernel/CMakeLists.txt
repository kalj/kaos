set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/link.ld)
set(CMAKE_EXE_LINKER_FLAGS "-static -z noexecstack -Wl,--build-id=none -T ${LINKER_SCRIPT}")

add_executable(kernel_elf 
    entry.asm 
    kmain.c
    tty.c
    uart.c 
    kaos.c
    pic.c
    keyboard.c
    floppy.c
    panic.c
    ide.c
    irq.c
    vga.c 
    i8254x.c
    cmos.c
    shell.c
    pci.c)
target_include_directories(kernel_elf PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(kernel_elf common_asm stdlib)
set_target_properties(kernel_elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

set_source_files_properties(pic.c PROPERTIES COMPILE_FLAGS "-mgeneral-regs-only -mno-red-zone")
set_source_files_properties(irq.c PROPERTIES COMPILE_FLAGS "-mgeneral-regs-only -mno-red-zone")
set_source_files_properties(keyboard.c PROPERTIES COMPILE_FLAGS "-mgeneral-regs-only -mno-red-zone")

set(KERNEL_BIN ${CMAKE_CURRENT_BINARY_DIR}/kernel.bin)
set(KERNEL_BIN ${KERNEL_BIN} PARENT_SCOPE)
add_custom_command(OUTPUT ${KERNEL_BIN}
    COMMAND objcopy -O binary $<TARGET_FILE:kernel_elf> ${KERNEL_BIN}
    DEPENDS $<TARGET_FILE:kernel_elf>)
add_custom_target(kernel
    ALL
    DEPENDS ${KERNEL_BIN})

