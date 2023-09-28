cmake_minimum_required(VERSION 3.6)

# set(CMAKE_ASM_COMPILER nasm)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)
set(CMAKE_ASM_NASM_FLAGS "-Wall")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -l <OBJECT>.lst -o <OBJECT> <SOURCE>")
set(CMAKE_C_FLAGS "-Wall -m32 -O1 -fno-builtin -nostdlib -ffreestanding -nostdinc -fno-pie -fno-asynchronous-unwind-tables")
project(kaos C ASM_NASM)

set(IMAGE_FILE main.img)

set(BOOTLOADER_CODE_START_OFFSET 62)
set(BOOTLOADER_SRC ${CMAKE_CURRENT_LIST_DIR}/bootloader.asm)
set(BOOTLOADER_BIN ${CMAKE_CURRENT_BINARY_DIR}/bootloader.bin)
set(BOOTLOADER_LST ${CMAKE_CURRENT_BINARY_DIR}/bootloader.lst)
set(POSTPROCESS_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/postprocess-bootloader.py)

add_custom_command(OUTPUT ${BOOTLOADER_BIN} ${BOOTLOADER_LST}
	COMMAND nasm -Wall -I ${CMAKE_CURRENT_LIST_DIR} -l ${BOOTLOADER_LST} -D CODE_START_OFFSET=${BOOTLOADER_CODE_START_OFFSET} -f bin -o ${BOOTLOADER_BIN} ${BOOTLOADER_SRC}
	COMMAND ${POSTPROCESS_SCRIPT} ${BOOTLOADER_BIN} ${BOOTLOADER_CODE_START_OFFSET} ${BOOTLOADER_LST}
    DEPENDS ${BOOTLOADER_SRC} ${POSTPROCESS_SCRIPT}
    )

add_custom_target(bootloader
    DEPENDS ${BOOTLOADER_BIN})

set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/link.ld)
set(CMAKE_EXE_LINKER_FLAGS "-static -z noexecstack -Wl,--build-id=none -T ${LINKER_SCRIPT}")

add_executable(kernel_elf kernel.c tty.c uart.c entry.asm kaos.c memory_map.c strfmt.c pic.c keyboard.c irq.c memcpy.c vga.c)
target_include_directories(kernel_elf PUBLIC ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(kernel_elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
set_source_files_properties(irq.c PROPERTIES COMPILE_FLAGS "-mgeneral-regs-only -mno-red-zone")
set_source_files_properties(keyboard.c PROPERTIES COMPILE_FLAGS "-mgeneral-regs-only -mno-red-zone")

set(KERNEL_BIN ${CMAKE_CURRENT_BINARY_DIR}/kernel.bin)
add_custom_command(OUTPUT ${KERNEL_BIN}
    COMMAND objcopy -O binary $<TARGET_FILE:kernel_elf> ${KERNEL_BIN}
    DEPENDS $<TARGET_FILE:kernel_elf>)

add_custom_target(image
    ALL
    DEPENDS ${IMAGE_FILE})

add_custom_command(OUTPUT ${IMAGE_FILE}
    COMMAND dd if=/dev/zero of=${IMAGE_FILE} count=1440 bs=1k status=none
	COMMAND mkfs.fat -F 12 -n "MY FLOPPY  " -i 0xcafe1234 ${IMAGE_FILE} 1440
	COMMAND dd if=${BOOTLOADER_BIN} obs=1 of=${IMAGE_FILE} seek=${BOOTLOADER_CODE_START_OFFSET} conv=notrunc status=none
	COMMAND mcopy -i ${IMAGE_FILE} ${KERNEL_BIN} ::/
    DEPENDS ${BOOTLOADER_BIN} ${KERNEL_BIN}
)

set(run_script ${CMAKE_CURRENT_LIST_DIR}/run.sh)
add_custom_target(run
    COMMAND ${run_script}
    DEPENDS ${IMAGE_FILE} ${run_script}
    USES_TERMINAL
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )

set(debug_script ${CMAKE_CURRENT_LIST_DIR}/debug.sh)
add_custom_target(debug
    COMMAND ${debug_script}
    DEPENDS ${IMAGE_FILE} ${debug_script}
    USES_TERMINAL
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
