ISO_DIR     = iso
BOOT_DIR    = bootloader
KERN_DIR    = kernel
BOOTLOADER  = $(BOOT_DIR)/build/bootloader.efi
KERNEL_BIN  = $(KERN_DIR)/build/kernel.bin
KERNEL_ELF  = $(KERN_DIR)/build/kernel.elf

# Docker Configuration
DOCKER_USER_FLAGS =
DOCKER_IMG  = tcvdh/mtos-builder
DOCKER_CMD  = docker run --rm ${DOCKER_USER_FLAGS} -v "$(shell pwd)":/root/os $(DOCKER_IMG)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    OVMF_CODE     = /opt/homebrew/share/qemu/edk2-x86_64-code.fd
    OVMF_VARS_SYS = /opt/homebrew/share/qemu/edk2-i386-vars.fd
else
    OVMF_CODE     = /usr/share/ovmf/x64/OVMF_CODE.4m.fd
    OVMF_VARS_SYS = /usr/share/ovmf/x64/OVMF_VARS.4m.fd
	DOCKER_USER_FLAGS = --user $(shell id -u):$(shell id -g)
endif

OVMF_VARS_LOCAL = $(BOOT_DIR)/build/OVMF_VARS.fd

QEMU_ACCEL = -cpu qemu64
QEMU_FLAGS = -machine q35 -m 2G \
             -drive if=pflash,format=raw,unit=0,file="$(OVMF_CODE)",readonly=on \
             -drive if=pflash,format=raw,unit=1,file="$(OVMF_VARS_LOCAL)" \
             -drive format=raw,file=fat:rw:$(ISO_DIR) \
             -net none \
             $(QEMU_ACCEL)

.PHONY: all build clean run debug docker-build docker-run usb FORCE

build: $(BOOTLOADER) $(KERNEL_BIN)

all: build
	@mkdir -p $(ISO_DIR)/EFI/BOOT
	cp $(BOOTLOADER) $(ISO_DIR)/EFI/BOOT/BOOTX64.EFI
	cp $(KERNEL_BIN) $(ISO_DIR)/kernel.bin
	@echo ">> Build Complete (Native)."

$(BOOTLOADER): FORCE
	$(MAKE) -C $(BOOT_DIR)

$(KERNEL_BIN): FORCE
	$(MAKE) -C $(KERN_DIR)

clean:
	$(MAKE) -C $(BOOT_DIR) clean
	$(MAKE) -C $(KERN_DIR) clean
	rm -f $(ISO_DIR)/kernel.bin $(ISO_DIR)/EFI/BOOT/BOOTX64.EFI
	rm -f $(OVMF_VARS_LOCAL)

docker-build:
	@echo ">> Compiling inside Docker..."
	$(DOCKER_CMD) make build

docker-run: docker-build run

docker-debug: docker-build debug

$(OVMF_VARS_LOCAL):
	@echo ">> Creating local copy of OVMF_VARS..."
	@mkdir -p $(dir $@)
	cp "$(OVMF_VARS_SYS)" "$@"
	chmod 644 "$@"

run: all $(OVMF_VARS_LOCAL)
	@echo ">> Launching QEMU..."
	qemu-system-x86_64 $(QEMU_FLAGS) -monitor stdio

debug: all $(OVMF_VARS_LOCAL)
	@echo ">> Launching QEMU in background (Debug Mode)..."
	@( \
		qemu-system-x86_64 $(QEMU_FLAGS) -s -S -no-reboot -no-shutdown -serial file:qemu.log >/dev/null 2>&1 & \
		QEMU_PID=$$! ; \
		echo ">> QEMU PID: $$QEMU_PID. Waiting for initialization..." ; \
		sleep 0.5 ; \
		echo ">> Connecting GDB..." ; \
		gdb -ex "target remote localhost:1234" \
		    -ex "set disassembly-flavor intel" \
		    -ex "layout split" \
		    -ex "layout regs" \
		    -ex "break *0x100000" \
		    -ex "continue" \
		    "$(KERNEL_ELF)" ; \
		echo ">> GDB Exited. Killing QEMU..." ; \
		kill $$QEMU_PID 2>/dev/null || true \
	)

usb: all
	sudo cp $(ISO_DIR)/EFI/BOOT/BOOTX64.EFI /mnt/usb/EFI/BOOT/BOOTX64.EFI
	sudo cp $(ISO_DIR)/kernel.bin /mnt/usb/kernel.bin
	sync
