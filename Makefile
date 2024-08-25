TARGET = aarch64-elf
CC = clang
LD = ld.lld
OBJCOPY = llvm-objcopy
CFLAGS = -target $(TARGET) -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T linker.ld -O2

SRC_DIR = src
BUILD_DIR = build
BOOT_SRC = $(SRC_DIR)/boot.S
KERNEL_SRC = $(SRC_DIR)/main.c
BOOT_OBJ = $(BUILD_DIR)/boot.o
KERNEL_OBJ = $(BUILD_DIR)/kernel.o
OUTPUT_ELF = os.elf
OUTPUT_IMG = kernel8.img

all: $(OUTPUT_IMG)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BOOT_OBJ): $(BOOT_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_OBJ): $(KERNEL_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT_ELF): $(BOOT_OBJ) $(KERNEL_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

$(OUTPUT_IMG): $(OUTPUT_ELF)
	$(OBJCOPY) $< -O binary $@

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_ELF) $(OUTPUT_IMG)

.PHONY: all clean
