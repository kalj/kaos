
IMAGE=main.img
BOOTLOADER_BIN=bootloader.bin
BOOTLOADER_SRC=bootloader.asm
BOOTLOADER_LST=bootloader.lst

$(IMAGE): $(BOOTLOADER_BIN)
	cp $< $@
	truncate -s 1440k $@

$(BOOTLOADER_BIN): $(BOOTLOADER_SRC)
	nasm -l $(BOOTLOADER_LST) -f bin -o $@ $<

clean:
	$(RM) $(IMAGE) $(BOOTLOADER_BIN)
