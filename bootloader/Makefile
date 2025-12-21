CC = gcc
LD = ld
OBJCOPY = objcopy
BUILD_DIR = build

EFIINC = /usr/include/efi
EFIINCS = -I$(EFIINC) -I$(EFIINC)/x86_64 -I$(EFIINC)/protocol
LIB = /usr/lib
EFILIB = /usr/lib
EFI_CRT_OBJS = $(EFILIB)/crt0-efi-x86_64.o
EFI_LDS = $(EFILIB)/elf_x86_64_efi.lds

OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/guid_utils.o

CFLAGS = $(EFIINCS) -fno-stack-protector -fpic \
         -fshort-wchar -mno-red-zone -Wall -std=c23 \
         -DEFI_FUNCTION_WRAPPER

LDFLAGS = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
          -Bsymbolic -L $(EFILIB) -L $(LIB) $(EFI_CRT_OBJS)

TARGET = $(BUILD_DIR)/bootloader.efi

all: $(TARGET)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/bootloader.so: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

$(BUILD_DIR)/%.efi: $(BUILD_DIR)/%.so
	$(OBJCOPY) -j .text -j .sdata -j .data -j .rodata -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-x86_64 $^ $@

clean:
	rm -rf $(BUILD_DIR)
