#!/bin/bash

cd bootloader
make clean
make
cd ..

mkdir -p iso/EFI/BOOT

cp bootloader/build/bootloader.efi iso/EFI/BOOT/BOOTX64.EFI


OVMF_CODE="/usr/share/ovmf/x64/OVMF_CODE.4m.fd"
OVMF_VARS_SYS="/usr/share/ovmf/x64/OVMF_VARS.4m.fd"
OVMF_VARS_LOCAL="bootloader/OVMF_VARS.fd"

if [ ! -f "$OVMF_VARS_LOCAL" ]; then
    echo "[*] Creating local copy of OVMF_VARS..."
    cp "$OVMF_VARS_SYS" "$OVMF_VARS_LOCAL"
    chmod 644 "$OVMF_VARS_LOCAL"
fi

# use `-cpu host` with `-enable-kvm` for native CPU
# use `-cpu qemu64` without `-enable-kvm` for emulated CPU
qemu-system-x86_64 \
    -cpu qemu64 \
    -drive if=pflash,format=raw,unit=0,file="$OVMF_CODE",readonly=on \
    -drive if=pflash,format=raw,unit=1,file="$OVMF_VARS_LOCAL" \
    -drive format=raw,file=fat:rw:iso \
    -net none \
    -monitor stdio \
