cmake_minimum_required(VERSION 3.15)

# set(CMAKE_ASM_COMPILER nasm)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf)
set(CMAKE_ASM_NASM_FLAGS "-Wall")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -l <OBJECT>.lst -o <OBJECT> <SOURCE>")
set(CMAKE_C_FLAGS "-Wall -m32 -O1 -fno-builtin -fno-stack-protector -nostdlib -ffreestanding -nostdinc -fno-pie -fno-asynchronous-unwind-tables")
project(kaos C ASM_NASM)

set(BOOTLOADER_CODE_START_OFFSET 62)

add_subdirectory(common_asm)
add_subdirectory(bootloader)
add_subdirectory(stdlib)
add_subdirectory(kernel)

set(IMAGE_FILE main.img)

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
