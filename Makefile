
IMAGE=main.img
BOOTLOADER_BIN=bootloader.bin
BOOTLOADER_SRC=bootloader.asm
BOOTLOADER_LST=bootloader.lst
KERNEL_BIN=kernel.bin
KERNEL_SRC=kernel.asm
KERNEL_LST=kernel.lst

$(IMAGE): $(BOOTLOADER_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$@ count=1440 bs=1k status=none
	mkfs.fat -F 12 -n "MY FLOPPY  " -i 0xcafe1234 $@ 1440
	dd if=$(BOOTLOADER_BIN) obs=1 of=$@ seek=62 conv=notrunc status=none
	mcopy -i $@ $(KERNEL_BIN) ::/

$(BOOTLOADER_BIN): $(BOOTLOADER_SRC)
	nasm -l $(BOOTLOADER_LST) -f bin -o $@ $<

$(KERNEL_BIN): $(KERNEL_SRC)
	nasm -l $(KERNEL_LST) -f bin -o $@ $<

clean:
	$(RM) $(IMAGE) $(BOOTLOADER_BIN) $(BOOTLOADER_LST) $(KERNEL_BIN) $(KERNEL_LST)
