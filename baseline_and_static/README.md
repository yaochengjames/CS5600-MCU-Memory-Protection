# Baseline and Static MPU Configuration

This directory contains two configurations using the **original FreeRTOS kernel** (unmodified).

---

## Configurations

### Baseline (No MPU)

**Purpose:** Establish performance baseline without any memory protection.

- MPU **disabled**
- All tasks share entire memory space (2.5 MB)
- No overhead from memory protection

**Build:**
```bash
make CONFIG=baseline
make CONFIG=baseline run
```

**Results:**
- Context switch: **83 cycles**
- Memory/task: **2.5 MB** (100%)
- Isolation: **None**

---

### Static MPU

**Purpose:** Measure overhead of fixed MPU configuration.

- MPU configured **once at boot time**
- All tasks share same 3 regions
- Memory regions never change at runtime

**Regions:**
- Region 0: Flash 0x00000000-0x00200000 (2 MB, RO+X)
- Region 1: SRAM 0x20000000-0x20080000 (512 KB, RW+XN)
- Region 2: Peripherals 0x40000000-0x50000000 (256 MB, RW+XN)

**Build:**
```bash
make CONFIG=static
make CONFIG=static run
```

**Results:**
- Context switch: **91 cycles** (+9.6% overhead)
- Memory/task: **2.5 MB** (100%, same as baseline)
- Isolation: **Coarse-grained** (prevents out-of-range access)

---

## Implementation Files

### Shared Files
- `src/main.c` - Main entry point (uses `CONFIG` macro to switch)
- `src/uart.c/h` - UART driver for output
- `src/benchmark.c/h` - SysTick-based cycle counter
- `src/test_tasks.c/h` - Performance measurement tasks
- `src/startup_cm4.S` - ARM Cortex-M4 startup code

### Static MPU Specific
- `src/mpu_static.c/h` - Static MPU configuration (only compiled for `CONFIG=static`)

### FreeRTOS
- `freertos/FreeRTOS-Kernel/` - **Unmodified** FreeRTOS v10.6

---

## Key Features

### Measurement Methodology
- **Tool:** SysTick timer (24-bit down counter)
- **Method:** `taskYIELD()` for pure context switch measurement
- **Samples:** 3,000 per configuration (after 200 warmup)
- **Filtering:** Outliers (< 10 or > 50,000 cycles) excluded

### MPU Fault Detection (Static only)
- **Test:** Write to Flash (read-only region)
- **Result:** System halts (fault triggered)
- **Verified:** GDB register inspection + runtime behavior

---

## Build System

**Makefile supports:**
- `make CONFIG=baseline` - Compile baseline
- `make CONFIG=static` - Compile static MPU
- `make CONFIG=baseline run` - Run baseline
- `make CONFIG=static run` - Run static
- `make CONFIG=static debug` - Debug with GDB
- `make clean` - Clean build artifacts

**Output:**
- `build/freertos_baseline.elf`
- `build/freertos_static.elf`

---

## Comparison

| Aspect | Baseline | Static MPU |
|--------|---------|-----------|
| MPU regions | 0 | 3 (shared) |
| Performance | 83 cycles | 91 cycles (+9.6%) |
| Memory/task | 2.5 MB | 2.5 MB (0% reduction) |
| Isolation | None | Basic segmentation |
| FreeRTOS mods | None | None |

**Conclusion:** Static MPU adds minimal overhead (~10%) but provides only basic protection. All tasks still share the same memory view.

---


