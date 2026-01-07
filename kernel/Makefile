CC = gcc
LD = ld
OBJCOPY = objcopy

CFLAGS = -ffreestanding -mno-red-zone -fno-stack-protector -fno-pic \
         -m64 -I../common -c -g

LDSCRIPT = linker.ld

BUILD_DIR = build
TARGET_ELF = $(BUILD_DIR)/kernel.elf
TARGET_BIN = $(BUILD_DIR)/kernel.bin

ASM_SRCS = $(wildcard *.S)
C_SRCS = $(wildcard *.c)
ASM_OBJS = $(patsubst %.S, $(BUILD_DIR)/%.o, $(ASM_SRCS))
C_OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRCS))

OBJS = $(ASM_OBJS) $(C_OBJS)

all: $(TARGET_BIN)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(TARGET_ELF): $(OBJS)
	$(LD) -T $(LDSCRIPT) -o $@ $(OBJS)

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)
