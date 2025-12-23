#!/bin/bash

# make clean
make


OVMF_CODE="/usr/share/ovmf/x64/OVMF_CODE.4m.fd"
OVMF_VARS_SYS="/usr/share/ovmf/x64/OVMF_VARS.4m.fd"
OVMF_VARS_LOCAL="bootloader/build/OVMF_VARS.fd"
KERNEL_ELF="kernel/build/kernel.elf"

if [ ! -f "$OVMF_VARS_LOCAL" ]; then
    echo "[*] Creating local copy of OVMF_VARS..."
    mkdir -p bootloader/build
    cp "$OVMF_VARS_SYS" "$OVMF_VARS_LOCAL"
    chmod 644 "$OVMF_VARS_LOCAL"
fi

QEMU_FLAGS=(
    -cpu qemu64
    -machine q35
    -m 2G
    -drive if=pflash,format=raw,unit=0,file="$OVMF_CODE",readonly=on
    -drive if=pflash,format=raw,unit=1,file="$OVMF_VARS_LOCAL"
    -drive format=raw,file=fat:rw:iso  # <--- Mounts your 'iso' folder as a USB stick
    -net none
)
if [ "$1" == "debug" ]; then
    echo "[*] Launching QEMU in background (Debug Mode)..."
    
    # Add debug flags:
    # -s: Listen on port 1234
    # -S: Freeze CPU at startup
    # -no-reboot: Don't restart on Triple Fault (so you can see the crash)
    QEMU_FLAGS+=(-s -S -no-reboot -no-shutdown -serial file:qemu.log)

    # Run QEMU in background &
    qemu-system-x86_64 "${QEMU_FLAGS[@]}" > /dev/null 2>&1 &
    QEMU_PID=$!

    # Kill QEMU when this script exits
    trap "kill $QEMU_PID 2>/dev/null" EXIT

    echo "[*] Waiting for QEMU..."
    sleep 0.5

    echo "[*] Connecting GDB to $KERNEL_ELF..."
    gdb -ex "target remote localhost:1234" \
        -ex "set disassembly-flavor intel" \
        -ex "layout split" \
        -ex "layout regs" \
        -ex "break kernel_entry" \
        -ex "continue" \
        "$KERNEL_ELF"

else
    # Normal Mode
    echo "[*] Launching QEMU..."
    QEMU_FLAGS+=(-monitor stdio)
    qemu-system-x86_64 "${QEMU_FLAGS[@]}"
fi

# use `-cpu host` with `-enable-kvm` for native CPU
# use `-cpu qemu64` without `-enable-kvm` for emulated CPU
# qemu-system-x86_64 \
#     -cpu qemu64 \
#     -machine q35 \
#     -drive if=pflash,format=raw,unit=0,file="$OVMF_CODE",readonly=on \
#     -drive if=pflash,format=raw,unit=1,file="$OVMF_VARS_LOCAL" \
#     -drive format=raw,file=fat:rw:iso \
#     -net none \
#     -monitor stdio \
