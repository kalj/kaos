CC=gcc
CFLAGS=-Wall -m32 -O1 -fno-builtin -nostdlib -fno-pie -fno-asynchronous-unwind-tables

IMAGE=main.img
BOOTLOADER_BIN=bootloader.bin
BOOTLOADER_SRC=bootloader.asm
BOOTLOADER_LST=bootloader.lst
KERNEL_BIN=kernel.bin
KERNEL_ASM=kernel.asm
KERNEL_SRC=kernel.c
KERNEL_OBJ=kernel.o
KERNEL_LST=kernel.lst
KERNEL_MAP=kernel.map
KERNEL_ELF=kernel.elf
LINK_SCRIPT=link.ld
BUILD_DIR=build

BOOTLOADER_CODE_START_OFFSET=62

KERNEL_OBJS=$(addprefix $(BUILD_DIR)/, entry.o kernel.o tty.o uart.o)


$(BUILD_DIR)/$(IMAGE): $(BUILD_DIR)/$(BOOTLOADER_BIN) $(BUILD_DIR)/$(KERNEL_BIN)
	dd if=/dev/zero of=$@ count=1440 bs=1k status=none
	mkfs.fat -F 12 -n "MY FLOPPY  " -i 0xcafe1234 $@ 1440
	dd if=$(BUILD_DIR)/$(BOOTLOADER_BIN) obs=1 of=$@ seek=$(BOOTLOADER_CODE_START_OFFSET) conv=notrunc status=none
	mcopy -i $@ $(BUILD_DIR)/$(KERNEL_BIN) ::/

$(BUILD_DIR)/$(BOOTLOADER_BIN): $(BOOTLOADER_SRC)
	nasm -Wall -l /tmp/$(BOOTLOADER_LST) -D CODE_START_OFFSET=$(BOOTLOADER_CODE_START_OFFSET) -f bin -o $@ $<
	./translate-bootloader-lst.py < /tmp/$(BOOTLOADER_LST) > $(BUILD_DIR)/$(BOOTLOADER_LST)
	@echo "*** Space left for bootloader: $$VAR$$((512 - 2 - $(BOOTLOADER_CODE_START_OFFSET) - $$(wc -c < $@) ))"


$(BUILD_DIR)/kernel.o: kernel.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD_DIR)/tty.o: tty.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD_DIR)/uart.o: uart.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD_DIR)/entry.o: entry.asm
	nasm -Wall -l $(BUILD_DIR)/entry.lst -f elf -o $@ $<

$(BUILD_DIR)/$(KERNEL_ELF): $(KERNEL_OBJS) $(LINK_SCRIPT)
	ld -m elf_i386 -z noexecstack -o $@ -T $(LINK_SCRIPT) -nostdlib -Map $(BUILD_DIR)/$(KERNEL_MAP) $(KERNEL_OBJS)

$(BUILD_DIR)/$(KERNEL_BIN): $(BUILD_DIR)/$(KERNEL_ELF)
	objcopy -O binary $< $@ 

.PHONY: run
run: $(BUILD_DIR)/$(IMAGE)
	@qemu-system-i386 -serial stdio -fda $^

.PHONY: debug
debug: $(BUILD_DIR)/$(IMAGE)
	gdb -x gdbscript.gdb

clean:
	$(RM) $(BUILD_DIR)/*
