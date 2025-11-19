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
    $(SRC_DIR)/uart.c \
    $(SRC_DIR)/benchmark.c \
    $(SRC_DIR)/test_tasks.c \
    $(SRC_DIR)/mpu_dynamic.c \
    $(FREERTOS_DIR)/tasks.c \
    $(FREERTOS_DIR)/list.c \
    $(FREERTOS_DIR)/queue.c \
    $(FREERTOS_DIR)/timers.c \
    $(FREERTOS_DIR)/event_groups.c \
    $(FREERTOS_DIR)/stream_buffer.c \
    $(FREERTOS_DIR)/portable/MemMang/heap_4.c \
    $(PORTABLE_DIR)/port.c

# ==== Output ====
ELF := $(BUILD_DIR)/freertos_dynamic.elf
BIN := $(BUILD_DIR)/freertos_dynamic.bin
MAP := $(BUILD_DIR)/freertos_dynamic.map

# ==== Flags ====
CFLAGS := -mcpu=cortex-m4 -mthumb -O2 -Wall \
          -ffreestanding \
          -I$(SRC_DIR) \
          -Ifreertos \
          -I$(FREERTOS_DIR)/include \
          -I$(PORTABLE_DIR) \
          -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
          -DCONFIG_DYNAMIC

LDFLAGS := -T link.ld -Wl,--gc-sections -Wl,-Map=$(MAP) --specs=nosys.specs

# ==== Rules ====
.PHONY: all clean run dump debug gdb

all: $(ELF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ELF): $(SRCS) | $(BUILD_DIR)
	@echo "=== Building Dynamic MPU configuration ==="
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(BIN)

dump: $(ELF)
	$(OBJDUMP) -d -S $(ELF) > $(BUILD_DIR)/freertos_dynamic.disasm

run: $(ELF)
	qemu-system-arm -M mps2-an386 -kernel $(ELF) -serial stdio -monitor none -nographic

clean:
	rm -rf $(BUILD_DIR)

# Debug with GDB
debug: $(ELF)
	@echo "Starting QEMU with GDB server on port 1234..."
	@echo "Configuration: Dynamic MPU"
	@echo "In another terminal, run: make gdb"
	qemu-system-arm -M mps2-an386 -kernel $(ELF) \
		-serial stdio -monitor none -nographic \
		-s -S

gdb: $(ELF)
	@echo "Connecting to QEMU GDB server..."
	@echo "Configuration: Dynamic MPU"
	arm-none-eabi-gdb $(ELF) \
		-ex "target remote localhost:1234" \
		-ex "break main" \
		-ex "continue"
