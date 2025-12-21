CC = gcc
LD = ld
OBJCOPY = objcopy

CFLAGS = -ffreestanding -mno-red-zone -fno-stack-protector -fno-pic \
         -m64 -I../common -c -g

LDSCRIPT = linker.ld

BUILD_DIR = build
TARGET_ELF = $(BUILD_DIR)/kernel.elf
TARGET_BIN = $(BUILD_DIR)/kernel.bin

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET_BIN)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(TARGET_ELF): $(OBJS)
	$(LD) -T $(LDSCRIPT) -o $@ $(OBJS)

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)
