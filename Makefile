ISO_DIR = iso
BOOT_DIR = bootloader
KERN_DIR = kernel

BOOTLOADER = $(BOOT_DIR)/build/bootloader.efi
KERNEL_BIN = $(KERN_DIR)/build/kernel.bin

.PHONY: all clean run usb FORCE

all: $(BOOTLOADER) $(KERNEL_BIN)
	@mkdir -p $(ISO_DIR)/EFI/BOOT
	cp $(BOOTLOADER) $(ISO_DIR)/EFI/BOOT/BOOTX64.EFI
	cp $(KERNEL_BIN) $(ISO_DIR)/kernel.bin
	@echo "Build Complete."

$(BOOTLOADER): FORCE
	$(MAKE) -C $(BOOT_DIR)

$(KERNEL_BIN): FORCE
	$(MAKE) -C $(KERN_DIR)

clean:
	$(MAKE) -C $(BOOT_DIR) clean
	$(MAKE) -C $(KERN_DIR) clean
	rm -f $(ISO_DIR)/kernel.bin $(ISO_DIR)/EFI/BOOT/BOOTX64.EFI

usb: all
	sudo cp $(ISO_DIR)/EFI/BOOT/BOOTX64.EFI /mnt/usb/EFI/BOOT/BOOTX64.EFI
	sudo cp $(ISO_DIR)/kernel.bin /mnt/usb/kernel.bin
	sync
