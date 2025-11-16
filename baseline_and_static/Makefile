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

# ==== Configuration ====
CONFIG ?= baseline

# ==== Sources ====
SRCS := \
    $(SRC_DIR)/main.c \
    $(SRC_DIR)/startup_cm4.S \
    $(SRC_DIR)/uart.c \
    $(SRC_DIR)/benchmark.c \
    $(SRC_DIR)/test_tasks.c \
    $(FREERTOS_DIR)/tasks.c \
    $(FREERTOS_DIR)/list.c \
    $(FREERTOS_DIR)/queue.c \
    $(FREERTOS_DIR)/timers.c \
    $(FREERTOS_DIR)/event_groups.c \
    $(FREERTOS_DIR)/stream_buffer.c \
    $(FREERTOS_DIR)/portable/MemMang/heap_4.c \
    $(PORTABLE_DIR)/port.c

# ==== Output ====
ELF := $(BUILD_DIR)/freertos_$(CONFIG).elf
BIN := $(BUILD_DIR)/freertos_$(CONFIG).bin
MAP := $(BUILD_DIR)/freertos_$(CONFIG).map

# ==== Flags ====
CFLAGS := -mcpu=cortex-m4 -mthumb -O2 -Wall \
          -ffreestanding \
          -I$(SRC_DIR) \
          -Ifreertos \
          -I$(FREERTOS_DIR)/include \
          -I$(PORTABLE_DIR) \
          -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
          -DCONFIG_$(shell echo $(CONFIG) | tr a-z A-Z)

LDFLAGS := -T link.ld -Wl,--gc-sections -Wl,-Map=$(MAP) --specs=nosys.sepecs

# ==== Rules ====
.PHONY: all clean run dump

all: $(ELF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ELF): $(SRCS) | $(BUILD_DIR)
	@echo "=== Building $(CONFIG) configuration ==="
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(BIN)

dump: $(ELF)
	$(OBJDUMP) -d -S $(ELF) > $(BUILD_DIR)/freertos_$(CONFIG).disasm

run: $(ELF)
	qemu-system-arm -M mps2-an386 -kernel $(ELF) -serial stdio -monitor none -nographic

clean:
	rm -rf $(BUILD_DIR)
