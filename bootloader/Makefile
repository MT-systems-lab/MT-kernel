# Compiler: MinGW for 64-bit Windows
CC = x86_64-w64-mingw32-gcc

BUILD_DIR = build

CFLAGS = -Wall -Wextra -std=c11 -fno-builtin -fno-stack-protector \
         -Wl,--subsystem,10 -e efi_main -nostdlib

INCLUDES = -I/usr/include/efi -I/usr/include/efi/x86_64 -I/usr/include/efi/protocol

TARGET = $(BUILD_DIR)/bootloader.efi

all: $(TARGET)

$(TARGET): main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)
