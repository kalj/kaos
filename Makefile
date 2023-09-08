
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

BOOTLOADER_CODE_START_OFFSET=62

$(IMAGE): $(BOOTLOADER_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$@ count=1440 bs=1k status=none
	mkfs.fat -F 12 -n "MY FLOPPY  " -i 0xcafe1234 $@ 1440
	dd if=$(BOOTLOADER_BIN) obs=1 of=$@ seek=$(BOOTLOADER_CODE_START_OFFSET) conv=notrunc status=none
	mcopy -i $@ $(KERNEL_BIN) ::/

$(BOOTLOADER_BIN): $(BOOTLOADER_SRC)
	nasm -l /tmp/$(BOOTLOADER_LST) -D CODE_START_OFFSET=$(BOOTLOADER_CODE_START_OFFSET) -f bin -o $@ $<
	./translate-bootloader-lst.py < /tmp/$(BOOTLOADER_LST) > $(BOOTLOADER_LST)

# $(KERNEL_BIN): $(KERNEL_ASM)
# 	nasm -l $(KERNEL_LST) -f bin -o $@ $<

$(KERNEL_OBJ): $(KERNEL_SRC)
	gcc -Wall -O1 -m16 -fno-builtin -nostdlib -fno-pie -fno-asynchronous-unwind-tables -c -o $@ $^

$(KERNEL_ELF): $(KERNEL_OBJ) $(LINK_SCRIPT)
	ld -m elf_i386 -o $@ -T $(LINK_SCRIPT) -Map $(KERNEL_MAP) $(KERNEL_OBJ)

$(KERNEL_BIN): $(KERNEL_ELF)
	objcopy -O binary $< $@ 

clean:
	$(RM) $(IMAGE) $(BOOTLOADER_BIN) $(BOOTLOADER_LST) $(KERNEL_BIN) $(KERNEL_ELF) $(KERNEL_OBJ) $(KERNEL_LST) $(KERNEL_MAP)
