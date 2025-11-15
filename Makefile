# ==== Toolchain ====
CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump
SIZE    := arm-none-eabi-size

# ==== Paths ====
BUILD_DIR := build
SRC_DIR   := src
FREERTOS_DIR := freertos/FreeRTOS-Kernel
PORTABLE_DIR := $(FREERTOS_DIR)/portable/GCC/ARM_CM4F

# ==== Sources ====
SRCS := \
    $(SRC_DIR)/main.c \
    $(SRC_DIR)/startup_cm4.S \
    $(FREERTOS_DIR)/tasks.c \
    $(FREERTOS_DIR)/list.c \
    $(FREERTOS_DIR)/queue.c \
    $(FREERTOS_DIR)/timers.c \
    $(FREERTOS_DIR)/event_groups.c \
    $(FREERTOS_DIR)/stream_buffer.c \
    $(FREERTOS_DIR)/portable/MemMang/heap_4.c \
    $(PORTABLE_DIR)/port.c

# ==== Output ====
ELF := $(BUILD_DIR)/freertos.elf
BIN := $(BUILD_DIR)/freertos.bin
MAP := $(BUILD_DIR)/freertos.map

# ==== Flags ====
CFLAGS := -mcpu=cortex-m4 -mthumb -O2 -Wall \
          -ffreestanding \
		  -Ifreertos \
          -I$(FREERTOS_DIR)/include \
          -I$(PORTABLE_DIR) \
          -mfpu=fpv4-sp-d16 -mfloat-abi=hard

LDFLAGS := -T link.ld -Wl,--gc-sections -Wl,-Map=$(MAP)

LIBS := -lc -lm -lnosys

# ==== Rules ====
.PHONY: all clean run dump

all: $(ELF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ELF): $(SRCS) | $(BUILD_DIR)
	@echo "=== Building: $@ ==="
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) $(LIBS) -o $@
	$(SIZE) $@

$(BIN): $(ELF)
	@echo "=== Generating BIN ==="
	$(OBJCOPY) -O binary $(ELF) $(BIN)

dump: $(ELF)
	$(OBJDUMP) -d -S $(ELF) > $(BUILD_DIR)/freertos.disasm

run: $(ELF)
	qemu-system-arm -M mps2-an386 -kernel $(ELF) -nographic

clean:
	rm -rf $(BUILD_DIR)

