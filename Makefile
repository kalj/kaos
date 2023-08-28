
IMAGE=main.img
BOOTLOADER_BIN=bootloader.bin
BOOTLOADER_SRC=bootloader.asm
BOOTLOADER_LST=bootloader.lst
KERNEL_BIN=kernel.bin
KERNEL_SRC=kernel.asm
KERNEL_LST=kernel.lst

$(IMAGE): $(BOOTLOADER_BIN) $(KERNEL_BIN)
	cp $(BOOTLOADER_BIN) $@
	truncate -s 1440k $@
	mcopy -i $@ $(KERNEL_BIN) ::/

$(BOOTLOADER_BIN): $(BOOTLOADER_SRC)
	nasm -l $(BOOTLOADER_LST) -f bin -o $@ $<

$(KERNEL_BIN): $(KERNEL_SRC)
	nasm -l $(KERNEL_LST) -f bin -o $@ $<

clean:
	$(RM) $(IMAGE) $(BOOTLOADER_BIN) $(BOOTLOADER_LST) $(KERNEL_BIN) $(KERNEL_LST)
